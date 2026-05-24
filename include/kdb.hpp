/* This file is part of the IEX2H5 project and is licensed under the MIT License.
 * 
 * Copyright © 2017–2025 Varga Consulting, Toronto, ON, Canada 🇨🇦
 * Contact: info@vargalabs.ca */

#pragma once

#ifdef HAVE_KDB

#include "consumers.hpp"
#include "k.h"
#include <vector>
#include <string>
#include <chrono>
#include <cstdint>
#include <algorithm>

namespace io::kdb {
    struct consumer_t : public io::base::consumer_t<consumer_t> {
        using base = io::base::consumer_t<consumer_t>;
        using typename base::clock, typename base::duration, typename base::time_point, typename base::contract_t;
        using base::T, base::contracts, base::rts;

        consumer_t(std::string uri, std::string, std::string, bool is_irts_enabled, bool is_rts_enabled)
            : base(is_irts_enabled, is_rts_enabled) {

            std::string host = "localhost";
            int port = 5000;

            if (uri.starts_with("kdb://"))
                uri = uri.substr(6);
            if (auto colon = uri.find(':'); colon != std::string::npos) {
                host = uri.substr(0, colon);
                port = std::stoi(uri.substr(colon + 1));
            } else if (!uri.empty()) {
                host = uri;
            }

            handle = khp((S)host.c_str(), port);
            if (handle < 0)
                THROW_RUNTIME_ERROR("kdb+ connection failed: " + host + ":" + std::to_string(port));

            K r = k(handle, "system \"mkdir -p ./iex2h5_db\"", (K)0);
            if (r) r0(r);

            db_size_before = query_dir_size();

            r = k(handle, "if[`ticks in key `.; delete ticks from `.]; ticks:([] time:`long$();sym:`symbol$();price:`float$();size:`int$();side:`char$())", (K)0);
            if (!r) {
                kclose(handle);
                THROW_RUNTIME_ERROR("kdb+ table init failed");
            }
            r0(r);

            INFO << "Connected to kdb+ backend: " << host << ":" << port << std::endl;
        }

        ~consumer_t() {
            if (handle >= 0)
                kclose(handle);
        }

        void on_day_begin(time_point day) {
            using namespace std::chrono;
            auto start_time = floor<seconds>(day);
            today = date::format("%F", floor<days>(day));

            K r = k(handle, "if[`ticks in key `.; delete ticks from `.]; ticks:([] time:`long$();sym:`symbol$();price:`float$();size:`int$();side:`char$())", (K)0);
            if (r) r0(r);

            status = iex::compat::format("▫ {}", start_time);
            std::cout << status << std::flush;
        }

        void append(time_point now, contract_t contract, float price, uint32_t size, char side) {
            time_buf.push_back(std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count());
            sym_buf.push_back(get_sym(contract));
            price_buf.push_back(price);
            size_buf.push_back(static_cast<int>(size));
            side_buf.push_back(static_cast<C>(side));
        }

        void on_trade_report(time_point time, contract_t id, float price, uint32_t size, uint8_t) {
            if (is_irts_enabled) append(time, id, price, size, 'T');
        }

        void on_ask(time_point time, contract_t id, float price, uint32_t size, uint8_t) {
            if (is_irts_enabled) append(time, id, price, size, 'A');
        }

        void on_bid(time_point time, contract_t id, float price, uint32_t size, uint8_t) {
            if (is_irts_enabled) append(time, id, price, size, 'B');
        }

        void on_heart_beat(time_point time) {
            flush();
            auto tp = date::format("%H:%M:%S", date::floor<std::chrono::seconds>(time));
            std::cout << clear << status << " " << tp << std::flush;
        }

        void on_day_end(time_point) {
            flush();

            std::string kdb_date = today;
            std::replace(kdb_date.begin(), kdb_date.end(), '-', '.');
            std::string q = "if[count ticks; `:./iex2h5_db/" + kdb_date + "/ticks/ set .Q.en[`:./iex2h5_db; ticks]; delete ticks from `. ]";

            K r = k(handle, (S)q.c_str(), (K)0);
            if (!r) {
                ERROR << "kdb+ splay failed (network)" << std::endl;
            } else if (r->t == -128) {
                ERROR << "kdb+ splay error: " << r->s << std::endl;
                r0(r);
            } else {
                r0(r);
            }
            std::cout << " ✓" << std::endl;
        }

        void on_session_end() {
            flush();
            if (!today.empty()) {
                std::string kdb_date = today;
                std::replace(kdb_date.begin(), kdb_date.end(), '-', '.');
                std::string q = "if[count ticks; `:./iex2h5_db/" + kdb_date + "/ticks/ set .Q.en[`:./iex2h5_db; ticks]; delete ticks from `. ]";
                K r = k(handle, (S)q.c_str(), (K)0);
                if (!r) {
                    ERROR << "kdb+ final splay failed (network)" << std::endl;
                } else if (r->t == -128) {
                    ERROR << "kdb+ final splay error: " << r->s << std::endl;
                    r0(r);
                } else {
                    r0(r);
                }
            }
            uint64_t db_size_after = query_dir_size();
            global::state::total_output_after = (db_size_after > db_size_before) ? (db_size_after - db_size_before) : 0;
            INFO << "Session ended. kdb+ delta: " << utils::human_readable(global::state::total_output_after) << std::endl;
        }

    private:
        void flush() {
            if (time_buf.empty()) return;

            size_t n = time_buf.size();
            K t = ktn(KJ, static_cast<int>(n));
            K s = ktn(KS, static_cast<int>(n));
            K p = ktn(KF, static_cast<int>(n));
            K z = ktn(KI, static_cast<int>(n));
            K d = ktn(KC, static_cast<int>(n));

            for (size_t i = 0; i < n; ++i) {
                kJ(t)[i] = time_buf[i];
                kS(s)[i] = ss((S)sym_buf[i].c_str());
                kF(p)[i] = price_buf[i];
                ((int*)kG(z))[i] = size_buf[i];
                kC(d)[i] = side_buf[i];
            }

            K data = knk(5, t, s, p, z, d);
            K r = k(handle, "`ticks insert", data, (K)0);

            if (!r) {
                ERROR << "kdb+ network error during flush" << std::endl;
            } else if (r->t == -128) {
                ERROR << "kdb+ error: " << r->s << std::endl;
                r0(r);
            } else {
                r0(r);
            }
            // k() consumes arguments; do not r0(data)

            global::state::event_count += static_cast<uint64_t>(n);

            time_buf.clear();
            sym_buf.clear();
            price_buf.clear();
            size_buf.clear();
            side_buf.clear();
        }

        const char* get_sym(contract_t id) {
            if (id >= sym_cache.size() || sym_cache.empty()) {
                sym_cache.resize(flatmap.size());
                for (uint64_t contract : flatmap) {
                    auto [symbol, idx] = utils::radix64::decode(contract);
                    if (idx < sym_cache.size())
                        sym_cache[idx] = symbol;
                }
            }
            return sym_cache[id].c_str();
        }

        uint64_t query_dir_size() {
            K r = k(handle, "system \"du -sb ./iex2h5_db 2>/dev/null | cut -f1\"", (K)0);
            uint64_t size = 0;
            if (r) {
                if (r->t == 0 && r->n > 0) {
                    K first = kK(r)[0];
                    if (first && first->t == KC) {
                        try {
                            std::string s((char*)kG(first), static_cast<size_t>(first->n));
                            size = std::stoull(s);
                        } catch (...) {}
                    }
                }
                r0(r);
            }
            return size;
        }

        int handle = -1;
        std::string today;
        uint64_t db_size_before = 0;
        std::vector<J> time_buf;
        std::vector<std::string> sym_buf;
        std::vector<F> price_buf;
        std::vector<int> size_buf;
        std::vector<C> side_buf;
        std::vector<std::string> sym_cache;
    };
}

#endif // HAVE_KDB
