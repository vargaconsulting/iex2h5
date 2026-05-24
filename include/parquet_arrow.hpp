/* This file is part of the IEX2H5 project and is licensed under the MIT License.
 * 
 * Copyright © 2017–2025 Varga Consulting, Toronto, ON, Canada 🇨🇦
 * Contact: info@vargaconsulting.ca */

#pragma once

#include "consumers.hpp"
#include <arrow/api.h>
#include <arrow/io/file.h>
#include <parquet/arrow/writer.h>
#include <filesystem>
#include <numeric>
#include <unordered_map>

namespace io::parquet_arrow {

namespace detail {

template<typename T>
inline T unwrap(arrow::Result<T> result) {
	if (!result.ok())
		THROW_RUNTIME_ERROR("Arrow error: " + result.status().ToString());
	return std::move(result).ValueUnsafe();
}

inline void check(const arrow::Status& status) {
	if (!status.ok())
		THROW_RUNTIME_ERROR("Arrow error: " + status.ToString());
}

} // namespace detail

struct tick_buffer_t {
	std::vector<uint64_t> time;
	std::vector<float>    price;
	std::vector<uint32_t> size;
	std::vector<uint16_t> flags;
};

struct consumer_t : public io::base::consumer_t<consumer_t> {
	using base = io::base::consumer_t<consumer_t>;
	using typename base::clock, base::duration, base::time_point, base::contract_t;
	using base::I, base::T, base::contracts, base::rts, base::CONTRACT_ID_MASK, base::flatmap;

	static constexpr size_t ROW_GROUP_FLUSH_THRESHOLD = 5'000'000;

	consumer_t(std::string dir, std::string asset_path, std::string tradingdays_path,
		bool is_irts_enabled, bool is_rts_enabled)
		: base(is_irts_enabled, is_rts_enabled),
		  dir(dir), asset_path(asset_path), tradingdays_path(tradingdays_path) {

		namespace fs = std::filesystem;
		if (!fs::exists(dir) || !fs::is_directory(dir))
			fs::create_directories(dir);
	}

	void on_day_begin(time_point day) {
		using namespace std::chrono;
		namespace fs = std::filesystem;
		auto start_time = floor<seconds>(day);

		try {
			today = date::format("%F", floor<days>(day));
			fs::path filepath = fs::path(dir) / "irts" / (today + ".parquet");
			if (!fs::exists(filepath.parent_path()))
				fs::create_directories(filepath.parent_path());

			if (fs::exists(filepath))
				fs::remove(filepath);

			auto out = detail::unwrap(arrow::io::FileOutputStream::Open(filepath.string()));

			parquet::WriterProperties::Builder props_builder;
			props_builder.compression(parquet::Compression::ZSTD);
			props_builder.enable_write_page_index();
			auto props = props_builder.build();

			arrow_schema = arrow::schema({
				arrow::field("symbol",       arrow::utf8()),
				arrow::field("time",         arrow::timestamp(arrow::TimeUnit::NANO)),
				arrow::field("price",        arrow::float32()),
				arrow::field("size",         arrow::uint32()),
				arrow::field("is_bid",       arrow::boolean()),
				arrow::field("is_trade",     arrow::boolean()),
				arrow::field("is_ask",       arrow::boolean()),
				arrow::field("remove_level", arrow::boolean())
			});

			arrow_writer = detail::unwrap(parquet::arrow::FileWriter::Open(
				*arrow_schema, arrow::default_memory_pool(), out, props));

			status = iex::compat::format("▫ {}", start_time);
		} catch (const std::runtime_error& err) {
			ERROR << err.what() << std::endl;
			status = iex::compat::format("⯑ {}", start_time);
		}

		per_symbol.clear();
		buffered_ticks = 0;
		std::cout << status << std::flush;
	}

	void append(time_point now, contract_t contract, float price, uint32_t size, uint16_t flags) {
		auto& buf = per_symbol[contract];
		buf.time.push_back(utils::to_ns(now));
		buf.price.push_back(price);
		buf.size.push_back(size);
		buf.flags.push_back(flags);
		buffered_ticks++;
		global::state::event_count++;
		if (buffered_ticks >= ROW_GROUP_FLUSH_THRESHOLD)
			flush_row_group();
	}

	void on_trade_report(time_point time, contract_t id, float price, uint32_t size, uint8_t) {
		if (is_irts_enabled) append(time, id, price, size, (1 << 1)); // IS_TRADE
	}

	void on_ask(time_point time, contract_t id, float price, uint32_t size, uint8_t) {
		if (is_irts_enabled) append(time, id, price, size, (1 << 2)); // IS_ASK
	}

	void on_bid(time_point time, contract_t id, float price, uint32_t size, uint8_t) {
		if (is_irts_enabled) append(time, id, price, size, (1 << 0)); // IS_BID
	}

	void on_heart_beat(time_point time) {
		auto tp = date::format("%H:%M:%S", date::floor<std::chrono::seconds>(time));
		std::cout << clear << status << " " << tp << std::flush;
	}

	void on_day_end(time_point) try {
		flush_row_group();
		if (arrow_writer)
			detail::check(arrow_writer->Close());
		arrow_writer.reset();
		std::cout << " ✓" << std::endl;
	} catch (const std::exception& err) {
		ERROR << err.what() << std::endl;
		std::cout << " ✗" << std::endl;
	}

	void on_session_end() {
		namespace fs = std::filesystem;
		try {
			fs::path path(dir + "/" + asset_path);
			if (fs::exists(path)) fs::remove(path);
			std::ofstream fd(path);
			if (!fd) THROW_RUNTIME_ERROR("Failed to open asset file: " + path.string());

			std::ranges::sort(flatmap, [](uint64_t a, uint64_t b) {
				return (a & CONTRACT_ID_MASK) < (b & CONTRACT_ID_MASK);
			});

			for (const auto& contract : flatmap)
				fd << utils::radix64::decode(contract).first << std::endl;
			fd.close();
		} catch (const std::exception& e) {
			ERROR << "Failed to write asset file: " << e.what() << '\n';
		}

		std::set<std::string> trading_days;
		for (const auto& entry : fs::directory_iterator(dir + "/irts")) {
			if (!entry.is_regular_file()) continue;
			auto name = entry.path().filename().string();
			if (name.size() == 14 && name.ends_with(".parquet")) {
				std::string date = name.substr(0, 10);
				trading_days.insert(date);
			}
		}

		if (!trading_days.empty()) {
			fs::path path(dir + "/" + tradingdays_path);
			std::ofstream fd(path);
			if (!fd) THROW_RUNTIME_ERROR("Failed to write trading days index: " + path.string());
			for (const auto& day : trading_days)
				fd << day << std::endl;
		}
	}

private:
	void flush_row_group() {
		if (per_symbol.empty() || buffered_ticks == 0) return;

		// Build contract_id → symbol lookup
		std::vector<std::string> id_to_symbol(flatmap.size());
		for (uint64_t entry : flatmap) {
			auto [symbol, idx] = utils::radix64::decode(entry);
			if (idx < id_to_symbol.size())
				id_to_symbol[idx] = utils::trim(symbol);
		}

		// Sort each instrument's buffer by time
		for (auto& [id, buf] : per_symbol) {
			std::vector<size_t> order(buf.time.size());
			std::iota(order.begin(), order.end(), 0);
			std::stable_sort(order.begin(), order.end(),
				[&](size_t a, size_t b) { return buf.time[a] < buf.time[b]; });

			tick_buffer_t sorted;
			sorted.time.reserve(buf.time.size());
			sorted.price.reserve(buf.price.size());
			sorted.size.reserve(buf.size.size());
			sorted.flags.reserve(buf.flags.size());
			for (size_t i : order) {
				sorted.time.push_back(buf.time[i]);
				sorted.price.push_back(buf.price[i]);
				sorted.size.push_back(buf.size[i]);
				sorted.flags.push_back(buf.flags[i]);
			}
			buf = std::move(sorted);
		}

		// Build Arrow arrays in symbol order
		arrow::MemoryPool* pool = arrow::default_memory_pool();
		arrow::StringBuilder symbol_builder(pool);
		arrow::TimestampBuilder time_builder(arrow::timestamp(arrow::TimeUnit::NANO), pool);
		arrow::FloatBuilder price_builder(pool);
		arrow::UInt32Builder size_builder(pool);
		arrow::BooleanBuilder is_bid_builder(pool);
		arrow::BooleanBuilder is_trade_builder(pool);
		arrow::BooleanBuilder is_ask_builder(pool);
		arrow::BooleanBuilder remove_level_builder(pool);

		detail::check(symbol_builder.Reserve(buffered_ticks));
		detail::check(time_builder.Reserve(buffered_ticks));
		detail::check(price_builder.Reserve(buffered_ticks));
		detail::check(size_builder.Reserve(buffered_ticks));
		detail::check(is_bid_builder.Reserve(buffered_ticks));
		detail::check(is_trade_builder.Reserve(buffered_ticks));
		detail::check(is_ask_builder.Reserve(buffered_ticks));
		detail::check(remove_level_builder.Reserve(buffered_ticks));

		for (contract_t id = 0; id < static_cast<contract_t>(id_to_symbol.size()); ++id) {
			auto it = per_symbol.find(id);
			if (it == per_symbol.end()) continue;

			const auto& buf = it->second;
			const std::string& sym = id_to_symbol[id];
			for (size_t i = 0; i < buf.time.size(); ++i) {
				detail::check(symbol_builder.Append(sym));
				detail::check(time_builder.Append(static_cast<int64_t>(buf.time[i])));
				detail::check(price_builder.Append(buf.price[i]));
				detail::check(size_builder.Append(buf.size[i]));
				uint16_t f = buf.flags[i];
				detail::check(is_bid_builder.Append((f & (1 << 0)) != 0));
				detail::check(is_trade_builder.Append((f & (1 << 1)) != 0));
				detail::check(is_ask_builder.Append((f & (1 << 2)) != 0));
				detail::check(remove_level_builder.Append((f & (1 << 3)) != 0));
			}
		}

		std::shared_ptr<arrow::Array> symbol_arr, time_arr, price_arr, size_arr;
		std::shared_ptr<arrow::Array> is_bid_arr, is_trade_arr, is_ask_arr, remove_level_arr;
		detail::check(symbol_builder.Finish(&symbol_arr));
		detail::check(time_builder.Finish(&time_arr));
		detail::check(price_builder.Finish(&price_arr));
		detail::check(size_builder.Finish(&size_arr));
		detail::check(is_bid_builder.Finish(&is_bid_arr));
		detail::check(is_trade_builder.Finish(&is_trade_arr));
		detail::check(is_ask_builder.Finish(&is_ask_arr));
		detail::check(remove_level_builder.Finish(&remove_level_arr));

		auto batch = arrow::RecordBatch::Make(arrow_schema, symbol_arr->length(), {
			symbol_arr, time_arr, price_arr, size_arr,
			is_bid_arr, is_trade_arr, is_ask_arr, remove_level_arr
		});

		detail::check(arrow_writer->WriteRecordBatch(*batch));

		per_symbol.clear();
		buffered_ticks = 0;
	}

	std::string dir, asset_path, tradingdays_path, today, status, clear = "\033[2K\r";
	std::unordered_map<contract_t, tick_buffer_t> per_symbol;
	size_t buffered_ticks = 0;
	std::shared_ptr<arrow::Schema> arrow_schema;
	std::unique_ptr<parquet::arrow::FileWriter> arrow_writer;
};

} // namespace io::parquet_arrow
