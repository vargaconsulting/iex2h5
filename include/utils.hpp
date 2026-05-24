/* This file is part of the IEX2H5 project and is licensed under the MIT License.
 * 
 * Copyright © 2017–2025 Varga Consulting, Toronto, ON, Canada 🇨🇦
 * Contact: info@vargaconsulting.ca */

#pragma once
#define ARMA_NO_DEBUG
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <cmath>
#include <stdexcept>
#include <cstdio>
#include <armadillo>
#include <date/date.h> 
#include <compat.hpp>
#include <filesystem>
#include <regex>
#include <unordered_set>
#include <fstream>
#include <algorithm>
#include <cctype>
namespace utils {
	namespace ch = std::chrono;
    
    inline uint64_t to_ns(std::chrono::system_clock::time_point tp) {
        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count()
        );
    }

    inline std::string iex_symbol(uint64_t iex_symbol) {
        return std::string(reinterpret_cast<const char*>(&iex_symbol), 8);
    }

    template <typename D>
    concept additive_range_type = requires(D d) { D{1}; d + d; d <= d; } && !std::is_same_v<D, std::string>;
    template<typename T> concept chrono_duration = requires { 
        typename T::rep;
	    typename T::period;
	    std::is_base_of_v<std::chrono::duration<typename T::rep, typename T::period>, T>;
    };

    template <int Precision, std::floating_point T>
    inline constexpr bool is_finite(T value) {
        constexpr T factor = std::pow(10.0, static_cast<T>(Precision));
        return std::ceil(value * factor) / factor > T{0.01};
    }

    template <typename T>
    requires std::floating_point<T>
    inline arma::uvec find_finite_diag(const arma::Mat<T>& Q) {
        std::vector<arma::uword> indices;
        indices.reserve(Q.n_rows);

        for (arma::uword i = 0; i < Q.n_rows; ++i)
            if (is_finite<4>(Q(i, i)))
                indices.push_back(i);

        return arma::uvec(indices);
    }
    inline std::chrono::sys_days string_to_day(const std::string& day){
        std::istringstream in{day};
        std::chrono::sys_days dp;
        date::from_stream(in, "%F", dp);
        return dp;
    }
    template <typename duration>
    inline duration string_to_duration(const std::string& time_str) {
        int h, m, s;
        char sep1, sep2;
        std::istringstream in(time_str);
        in >> h >> sep1 >> m >> sep2 >> s;
    
        if (!in || sep1 != ':' || sep2 != ':' || in.peek() != EOF)
            throw std::runtime_error("Invalid time format: " + time_str);
    
        return ch::duration_cast<duration>(ch::hours{h} + ch::minutes{m} + ch::seconds{s});
    }

    template <typename duration_t, typename... strings_t>
    requires (std::convertible_to<strings_t, std::string> && ...)
    std::tuple<duration_t, duration_t, duration_t> strings_to_duration(strings_t&&... strs) {
        return std::make_tuple(string_to_duration<duration_t>(std::forward<strings_t>(strs))...);
    }

    template <typename duration_t>
    std::vector<duration_t> string_to_duration(const std::vector<std::string>& time) {
        std::vector<duration_t> result;
        result.reserve(time.size());
        for (const auto& str : time) 
            result.push_back(string_to_duration<duration_t>(str));
        return result;
    }

    template <typename duration>
    inline std::string duration_to_string(const duration& dur) {
        using namespace std::chrono;
        return date::format("%H:%M:%S", date::floor<seconds>(dur));
    }

	template <additive_range_type D>
	inline constexpr std::vector<D> sequence(D begin, D interval, D end) {
		std::vector<D> result;
        for (D current = begin; current <= end; current += interval)
			result.push_back(current);
		return result;
	}

    template <chrono_duration duration_t>
    inline std::vector<std::string> 
    sequence(const std::string& begin, const std::string& interval, const std::string& end) {
        using namespace std::chrono;
        auto [b, i, e] = utils::strings_to_duration<duration_t>(begin, interval, end);
        std::vector<std::string> result;
        for (auto d = b + i; d <= e; d += i)
            result.push_back(duration_to_string(d));
        return result;
    }

    template <typename duration_t>
    duration_t require_uniform_interval(const std::vector<duration_t>& times) {
        duration_t interval = times[1] - times[0];
        if (times.size() < 2) return interval;
        for (std::size_t i = 2; i < times.size(); ++i) {
            if (times[i] - times[i - 1] != interval)
                throw std::runtime_error("Non-uniform interval at index " + std::to_string(i));
        }
        return interval;
    }
        
    inline bool is_gzip(FILE* fd) {
        if (!fd) return false;

        unsigned char magic[2];
        long pos = std::ftell(fd);  // Save current position
        if (std::fread(magic, 1, 2, fd) != 2)
            return false;
        std::fseek(fd, pos, SEEK_SET);  // Rewind to original pos

        return magic[0] == 0x1F && magic[1] == 0x8B;
    }

    std::vector<std::string> expand_glob(const std::string& pattern) {
        namespace fs = std::filesystem;
        std::vector<std::string> result;
    
        const auto slash_pos = pattern.find_last_of("/\\");
        const std::string dir  = (slash_pos != std::string::npos) ? pattern.substr(0, slash_pos) : ".";
        const std::string glob = (slash_pos != std::string::npos) ? pattern.substr(slash_pos + 1) : pattern;
    
        std::string regex_str = std::regex_replace(
            glob, std::regex(R"([\.\^\$\|\(\)\[\]\+\{\}\\])"), R"(\\$&)"
        );
        regex_str = std::regex_replace(regex_str, std::regex(R"(\*)"), ".*");
        regex_str = std::regex_replace(regex_str, std::regex(R"(\?)"), ".");
    
        const std::regex pattern_regex(regex_str);
    
        for (const auto& entry : fs::directory_iterator(dir)) {
            const std::string filename = entry.path().filename().string();
            if (std::regex_match(filename, pattern_regex))
                result.push_back(entry.path().string());
        }
    
        return result;
    }

    std::vector<std::string> expand_glob_recursive(const std::string& pattern) {
        namespace fs = std::filesystem;
        std::vector<std::string> result;

        const auto slash_pos = pattern.find_last_of("/\\");
        const std::string dir  = (slash_pos != std::string::npos) ? pattern.substr(0, slash_pos) : ".";
        const std::string glob = (slash_pos != std::string::npos) ? pattern.substr(slash_pos + 1) : pattern;

        std::string regex_str = std::regex_replace(glob, std::regex(R"([\.\^\$\|\(\)\[\]\+\{\}\\])"), R"(\\$&)");

        regex_str = std::regex_replace(regex_str, std::regex(R"(\*\*/?)"), R"(.*?/)");
        regex_str = std::regex_replace(regex_str, std::regex(R"(\*)"), R"([^/]*?)");
        regex_str = std::regex_replace(regex_str, std::regex(R"(\?)"), R"([^/])");

        const std::regex pattern_regex(regex_str);

        for (const auto& entry : fs::recursive_directory_iterator(dir)) {
            if (!fs::is_regular_file(entry)) continue;

            const std::string relative_path = fs::relative(entry.path(), dir).string();
            if (std::regex_match(relative_path, pattern_regex))
                result.push_back(entry.path().string());
        }

        return result;
    }
    
    std::vector<std::string> resolve_input_paths(const std::vector<std::string>& raw_inputs) {
        namespace fs = std::filesystem;
        std::vector<std::string> files;
        for (const auto& filename : raw_inputs) {
            if (filename == "-" || fs::is_regular_file(filename))
                files.emplace_back(filename);
            else if (fs::is_directory(filename)) {
                for (const auto& entry : fs::directory_iterator(filename))
                    if (fs::is_regular_file(entry))
                        files.emplace_back(entry.path().string());
            } else if (filename.find("**") != std::string::npos)  {
                auto matches = expand_glob_recursive(filename);
                files.insert(files.end(), matches.begin(), matches.end());
            } else if (filename.find('*') != std::string::npos || filename.find('?') != std::string::npos) {
                auto matches = expand_glob(filename);
                files.insert(files.end(), matches.begin(), matches.end());
            } else files.emplace_back(filename);
        }
        return files;
    }

    inline std::uintmax_t path_size(std::filesystem::path const& path) {
        namespace fs = std::filesystem;
        std::uintmax_t total = 0;
        if (fs::is_regular_file(path)) return fs::file_size(path);
        if (fs::is_directory(path))
            for (auto const& entry : fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied))
                if (entry.is_regular_file()) total += entry.file_size();
        return total;
    }

    inline std::uintmax_t path_size(std::string const& pattern) {
        namespace fs = std::filesystem;
        if (fs::is_directory(pattern))
            return path_size(fs::path{pattern});
        auto files = resolve_input_paths(std::vector<std::string>{pattern});
        std::uintmax_t total = 0;
        for (auto const& fp : files)
            total += path_size(fs::path{fp});
        return total;
    }

    inline std::uintmax_t path_size(std::vector<std::string> const& patterns) {
        std::uintmax_t total = 0; 
        for (auto const& pat : patterns)
            total += path_size(pat); 
        return total;
    }    
    inline std::string human_readable(std::uintmax_t b) {
        static constexpr std::array<char const*,4> units{"B","KiB","MiB","GiB"};
        double v = double(b);
        int u = 0;
        while (v >= 1024.0 && u < 3)
            v /= 1024.0, ++u;
        return fmt::format("{:.2f} {}", v, units[u]);
    }
} // namespace util

namespace utils::pcap {
    inline constexpr uint32_t MAGIC_NATIVE_USEC = 0xa1b2c3d4;
    inline constexpr uint32_t MAGIC_NATIVE_NSEC = 0xa1b23c4d;
    inline constexpr uint32_t MAGIC_SWAP_USEC = 0xd4c3b2a1;
    inline constexpr uint32_t MAGIC_SWAP_NSEC = 0x4d3cb2a1;

    inline bool is_little_endian(uint32_t magic) {
        if (std::endian::native == std::endian::little){
            return (magic == 0xa1b2c3d4 || magic == 0xa1b23c4d);
        } else return (magic == 0xd4c3b2a1 || magic == 0x4d3cb2a1); 
    }

    inline bool is_big_endian(uint32_t magic) {
        if (std::endian::native == std::endian::big){
            return (magic == 0xa1b2c3d4 || magic == 0xa1b23c4d);
        } else return (magic == 0xd4c3b2a1 || magic == 0x4d3cb2a1); 
    } 

    inline bool is_native_byte_order(uint32_t magic) {
        return magic == 0xa1b2c3d4 || magic == 0xa1b23c4d;
    }

    inline bool needs_byteswap(uint32_t magic) {
        return !is_native_byte_order(magic);
    }

    inline bool is_valid_magic(uint32_t magic) {
        return magic == MAGIC_NATIVE_USEC || magic == MAGIC_NATIVE_NSEC ||
            magic == MAGIC_SWAP_USEC || magic == MAGIC_SWAP_NSEC;
    }
} // namespace utils::pcap

namespace utils::pcapng {
    /** @ingroup utils_pcap
     *  @brief PCAPNG magic number used in Section Header Block (SHB)
     *         to identify the file format. Always little endian.
     */
    inline constexpr uint32_t MAGIC = 0x0A0D0D0A;

    /** @ingroup utils_pcap
     *  @brief Offsets within SHB for fields relevant to byte-order detection. */
    enum : size_t {
        SHB_MAGIC_OFFSET        = 0,  /*!< offset of magic number */
        SHB_TOTAL_LENGTH_OFFSET = 4,  /*!< offset of total length field */
        SHB_BYTE_ORDER_MAGIC_OFFSET = 8 /*!< offset of byte-order magic */
    };

    /** @ingroup utils_pcap
     *  @brief PCAPNG byte-order magic (same bytes as pcap nanosecond magic).
     *         Used at offset 8 in SHB to detect endianness.
     */
    inline constexpr uint32_t BYTE_ORDER_MAGIC_LE = 0x1A2B3C4D;
    inline constexpr uint32_t BYTE_ORDER_MAGIC_BE = 0x4D3C2B1A;
    /** @ingroup utils_pcap
     *  @brief Checks whether the given value matches the PCAPNG magic number.
     *  @param magic 32-bit value typically found at the start of a PCAPNG file.
     *  @return true if the value equals `0x0A0D0D0A`, false otherwise.
     */
    inline bool is_valid_magic(uint32_t magic) {
        return magic == MAGIC;
    }
    /** @ingroup utils_pcap
     *  @brief Determines if the PCAPNG byte-order magic corresponds to little-endian.
     *  @param byte_order_magic 32-bit value found at offset 0x08 in the SHB.
     *  @return true if the byte order magic is `0x1A2B3C4D`, false otherwise.
     */
    inline bool is_little_endian(uint32_t byte_order_magic) {
        return byte_order_magic == BYTE_ORDER_MAGIC_LE;
    }
    /** @ingroup utils_pcap
     *  @brief Determines if the PCAPNG byte-order magic corresponds to big-endian.
     *  @param byte_order_magic 32-bit value found at offset 0x08 in the SHB.
     *  @return true if the byte order magic is `0x4D3C2B1A`, false otherwise.
     */
    inline bool is_big_endian(uint32_t byte_order_magic) {
        return byte_order_magic == BYTE_ORDER_MAGIC_BE;
    }
    /** @ingroup utils_pcap
     *  @brief Validates whether the given byte-order magic is recognized.
     *  @param byte_order_magic 32-bit value from the Section Header Block.
     *  @return true if the byte order magic matches either LE or BE constants.
     */
    inline bool is_valid_byte_order_magic(uint32_t byte_order_magic) {
        return is_little_endian(byte_order_magic) || is_big_endian(byte_order_magic);
    }
    /** @ingroup utils_pcap
     *  @brief Determines if the byte-order magic matches the host's native endianness.
     *  @param byte_order_magic 32-bit value from the Section Header Block.
     *  @return true if host and file endianness match, false if byteswapping is needed.
     */
    inline bool is_native_byte_order(uint32_t byte_order_magic) {
        if constexpr (std::endian::native == std::endian::little)
            return byte_order_magic == BYTE_ORDER_MAGIC_LE;
        else
            return byte_order_magic == BYTE_ORDER_MAGIC_BE;
    }
    /** @ingroup utils_pcap
     *  @brief Checks if the captured PCAPNG block requires byte-swapping on this host.
     *  @param byte_order_magic 32-bit value from the SHB block.
     *  @return true if the byte order differs from host endian, false otherwise.
     */
    inline bool needs_byteswap(uint32_t byte_order_magic) {
        return !is_native_byte_order(byte_order_magic);
    }

} // namespace utils::pcapng

namespace utils {
    inline std::string trim(const std::string& str) {
        auto front = std::find_if_not(str.begin(), str.end(), [](int c) { return std::isspace(c); });
        auto back  = std::find_if_not(str.rbegin(), str.rend(), [](int c) { return std::isspace(c); }).base();
        return (back <= front ? std::string() : std::string(front, back));
    }

    inline std::string pad(const std::string& str, std::size_t width, char pad_char = ' ') {
        if (str.size() > width)
            throw std::invalid_argument("Input string longer than target width");
        std::string padded = str;
        padded.resize(width, pad_char);
        return padded;
    }

    inline std::vector<std::string> trim(std::vector<std::string> from) {
        std::transform(from.begin(), from.end(), from.begin(), [](std::string& element) {
            return utils::trim(element);
        });
        return from;
    }

    inline std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        if (s.empty()) return tokens;
        std::string token;
        std::istringstream token_stream(s);
        while (std::getline(token_stream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    inline std::string join(const std::vector<std::string>& v, const std::string& delimiter) {
        return std::accumulate(std::begin(v), std::end(v), std::string(),
            [&delimiter](const std::string& a, const std::string& b) -> std::string {
                return a + (a.length() > 0 ? delimiter : "") + b;
            });
    }

    inline std::string replace(std::string str, const char a, const char b) {
        for (auto& c : str) if (c == a) c = b;
        return str;
    }

    template <class T>
    inline void unique(T& container) {
        std::sort(container.begin(), container.end());
        auto last = std::unique(container.begin(), container.end());
        container.erase(last, container.end());
    }

    template<typename T>
    inline std::vector<T> merge(const std::vector<T>& a, const std::vector<T>& b) {
        std::unordered_set<T> seen(a.begin(), a.end());
        std::vector<T> result = a;
        for (const auto& item : b) {
            if (seen.insert(item).second)
                result.push_back(item);
        }
        return result;
    }

    inline std::string to_lower(const std::string& str) {
        std::string result; result.reserve(str.size());
        std::ranges::transform(str, std::back_inserter(result),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    inline std::string to_lower(std::string& str) {
        std::ranges::transform(str, str.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return str;
    }

    inline std::string to_upper(const std::string& str) {
        std::string result; result.reserve(str.size());
        std::ranges::transform(str, std::back_inserter(result),
            [](unsigned char c) { return std::toupper(c); });
        return result;
    }

    inline std::string to_upper(std::string& str) {
        std::ranges::transform(str, str.begin(),
            [](unsigned char c) { return std::toupper(c); });
        return str;
    }

    inline std::string env2redis_key(std::string str) {
        to_lower(str);
        std::ranges::replace(str, '_', '-');
        return str;
    }

    inline std::string mask_out(std::string str, char mask = 'x', int count = 4) {
        if (str.size() <= 2 * count)
            return std::string(str.size(), mask);
        std::fill(str.begin() + count, str.end() - count, mask);
        return str;
    }

    inline std::string today() {
        auto today = date::floor<date::days>(std::chrono::system_clock::now());
        return date::format(":%F", today);
    }

    inline std::chrono::system_clock::duration time_until_midnight_utc() {
        auto now = std::chrono::system_clock::now();
        std::time_t current_time = std::chrono::system_clock::to_time_t(now);
        std::tm* utc_time = std::gmtime(&current_time);
        int seconds_since_midnight = utc_time->tm_hour * 3600 + utc_time->tm_min * 60 + utc_time->tm_sec;
        constexpr int seconds_per_day = 24 * 3600;
        int remaining_seconds = seconds_per_day - seconds_since_midnight;
        return std::chrono::seconds(remaining_seconds);
    }
} // namespace utils


namespace file {
    std::string detect_format(const std::string& path) {
        namespace fs = std::filesystem;
        using namespace std::literals;
    
        if (fs::exists(path)) {
            std::ifstream file(path, std::ios::binary);
            if (!file)
                throw std::runtime_error("Failed to open file: " + path);
    
            char magic[8] = {};
            file.read(magic, sizeof(magic));
    
            if (std::memcmp(magic, "\211HDF\r\n\032\n", 8) == 0) return "hdf5";
            if ((uint8_t)magic[0] == 0x1F && (uint8_t)magic[1] == 0x8B) {
                std::string ext = utils::to_lower(fs::path(path).extension().string());
                if (ext.ends_with(".gz") || ext.ends_with(".gzip"))
                    return "csv.gz"; // could refine further
            }
            std::string line;
            file.seekg(0);
            if (std::getline(file, line)) {
                if (line.find(',') != std::string::npos) return "csv";
                if (line.find('\t') != std::string::npos) return "tsv";
            }
        }
        const std::string ext = utils::to_lower(fs::path(path).extension().string());
    
        if (ext == ".h5" || ext == ".hdf5") return "hdf5";
        if (ext == ".csv") return "csv";
        if (ext == ".tsv") return "tsv";
        if (ext == ".json") return "json";
    
        if (path.starts_with("redis://")) return "redis";
        if (path.starts_with("mysql://")) return "mysql";
    
        throw std::runtime_error("Unable to detect format: " + path);
    }    
}
namespace utils::pair {
    std::pair<std::string, std::string> split(const std::string& str, char delimiter) {
        size_t pos = str.find(delimiter);
        if (pos != std::string::npos)
            return {str.substr(0, pos), str.substr(pos + 1)};
        return {str, ""};
    }
}

namespace utils::parse {
    std::pair<std::string,std::string> time_interval(std::string interval){
        return utils::pair::split(interval, '-');
    }
    std::pair<std::string,std::string> date_interval(std::string interval){
        std::pair<std::string,std::string> date = utils::pair::split(interval, ':');
        if( to_lower(date.second) == "today") 
            date.second = date::format("%F", floor<std::chrono::days>(std::chrono::system_clock::now()));
        return date;        
    }    
}