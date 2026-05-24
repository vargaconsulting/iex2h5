/* This file is part of the IEX2H5 project and is licensed under the MIT License.
 * 
 * Copyright © 2017–2025 Varga Consulting, Toronto, ON, Canada 🇨🇦
 * Contact: info@vargaconsulting.ca */

#include <string>
#include <iostream>
#include <string>
#include <csignal>

#include <argparse>
#include <error.hpp>
#include <utils.hpp>
#include <patterns.hpp>
#include <producers.hpp>
#include <consumers.hpp>
#include <hdf5.hpp>
#include <csv.hpp>
#include <redis.hpp>
#include <radix64.hpp>
#include <io.hpp>
#include <licenses.hpp>
#include <global_state.hpp>
#include <json.hpp>
#ifdef IEX2H5_HAVE_PARQUET
#include <parquet_arrow.hpp>
#endif

#ifndef IEX_MAX_SYMBOLS
	#define IEX_MAX_SYMBOLS 1 << 16
#endif
void signal_handler(int signal) {
	INFO << "received signal: " << signal << ", initiating shutdown..." << std::endl;
	global::state::shutdown_requested.store(true);
}

int main(int argc, char **argv) {
	using std::cout, std::cerr, std::endl;
	
	std::string output_path_or_url, rts_path, instruments_path, trading_days_path, days, interval, time_range, date_range, convert, benchmark_format,
		copyright = "Copyright © 2017–2025 Varga Consulting, Toronto, ON, Canada   info@vargaconsulting.ca",
		iex_attribution = "\033[1m[iex2h5]\033[0m Market data © IEX — Investors Exchange. Attribution required. See https://iextrading.com";

    unsigned compression_level;
	std::string version( "\033[1m" IEX2H5_SOFTWARE_VERSION "\033[0m" " commit: "  IEX2H5_SOFTWARE_COMMIT_HASH);
	argparse::ArgumentParser program(argv[0], version, argparse::default_arguments::none);
	program.add_argument("-h", "--help")
	.action([&](const std::string& s) {
		cout << "\033[1m" "IEX2H5 converts IEX TOPS Datasets to HDF5 Format" "\033[0m" << endl << endl;
		cout << "iex2h5 is a specialized tool for importing IEX TOPS datasets into the HDF5 format,"
			"enabling efficient storage and analysis of large-scale financial data. HDF5 is a widely"
			"adopted format for managing hierarchical, structured data and is supported across major"
			"environments such as Julia, Python, MATLAB, C++, and Node.js.\n\n"

			"This tool allows users to convert captured packet data streams (e.g. IEX DEEP/TOPS) into"
			"structured HDF5 datasets for quantitative analysis, visualization, and integration with"
			"scientific, engineering, or trading workflows." << endl;
		cout << program << endl << endl;
		cout << "\033[1m" "Examples:" "\033[0m" <<endl;
		cout << "   " << argv[0] << " -o ~/iex.h5 -c irts ~/data/202{4,5}-{04,05}-??.pcap.gz # Convert gzipped PCAP files to IRTS (brace expansion and globs supported)" << std::endl;
		cout << "   " << argv[0] << " -o rts.h5  --time-interval 00:00:10 -c rts iex.h5      # Load IRTS from HDF5 and convert to RTS matrices at 10-seconds intervals" << endl;
		cout << "   " << argv[0] << " -o ~/iex.h5 -c irts ~/data/**/*.pcap                   # Convert plain PCAP files to IRTS tickdata and store in HDF5 format" << endl;
		cout << "   " << argv[0] << " -o ~/out.csv -c irts ~/data/**/*.pcap                  # Convert plain PCAP files to IRTS tickdata and store in directory of CSV files" << endl;
		cout << "   " << argv[0] << " -o rts.h5  --time-interval 00:05:00 -c rts *.pcap.gz   # Load IRTS from HDF5 and convert to RTS matrices at 5-minutes intervals" << endl;
		cout << endl;
		cout << "\033[1m[iex2h5]\033[0m Market data © IEX — Investors Exchange. Attribution required. See https://iextrading.com" << std::endl;
		cout << copyright << endl << endl;
		std::exit(0);
	})
	.default_value(false)
	.help("shows help message")
	.implicit_value(true)
	.nargs(0);
	
	program.add_argument("--version").help("Print version information").default_value(false).implicit_value(true)
	.nargs(0).action([&](const std::string&) {
        cout << "\n\033[1m" << argv[0] << "\033[0m" << " " << version << "\n\n"
		<< "IEX2H5: High-performance IEX market data importer\n"
		<< "Converts DEEP/TOPS pcap captures into HDF5 datasets\n\n"
		<< "Copyright © 2017–2025 Varga Consulting, Toronto, ON, Canada   info@vargaconsulting.ca\n"
		<< "All rights reserved. info@vargaconsulting.ca\n"
		<< "Licensed under the MIT License.\n" << endl;
        std::exit(0);
    });
	program.add_argument("--time-interval").default_value(std::string("00:01:00")).help("temporal interval in hh::mm::ss format, irts stream is converted into");
	program.add_argument("--time-range").default_value(std::string("14:30:00-21:00:00")).help("Time window in UTC, specified as START-END (e.g. 14:30:00-21:00:00). Events outside this range are ignored.");
	program.add_argument("--date-range").default_value(std::string("2016-12-01:today")).help("Inclusive trading date range in format START:END (e.g. 2016-12-01:2020-01-01). Use 'today' as a valid END value.");

	program.add_argument("-o", "--output").default_value(std::string("./iex.h5")).help("path to the HDF5 container");
	
	program.add_argument("--rts-path").default_value(std::string("/time.txt")).help("HDF5 path for regular time index");
	program.add_argument("--instruments-path").default_value(std::string("/instruments.txt")).help("HDF5 path for instrument (symbol) list");
	program.add_argument("--trading-days-path").default_value(std::string("/trading_days.txt")).help("HDF5 path for trading day index");
	program.add_argument("-g", "--gzip").default_value(static_cast<unsigned>(1)).scan<'u', unsigned>().help("Compression level (0 = none, 9 = maximum)");
	
	program.add_argument("-c", "--convert").default_value(std::string("all")).choices("rts", "irts", "all", "none").help("Which conversion pipeline to run: rts | irts | none | all");

	program.add_argument("remaining").remaining();
	program.add_argument("--third-party-licenses").nargs('*').default_value(std::vector<std::string>{"all"}).implicit_value("all")
		.help("Print license(s) for a third-party library (or 'all | license 01 [, license 02, ...]')");
	program.add_argument("--benchmark-format").default_value(std::string("human")).choices("human", "csv").help("output format for benchmark line: human or csv");
	try {
		program.parse_args(argc, argv);
	} catch (const std::exception& err){
		std::cerr << err.what() << std::endl;
		return 1;
	}
	if (program.is_used("--third-party-licenses")) {
		std::map<std::string, std::string> license_map;
		for (const auto& [name, _, content] : licenses::thirdparty)
			license_map[name] = content;
		if (program.is_used("remaining") ){
			std::vector remaining = program.get<std::vector<std::string>>("remaining");
			if(remaining.empty() || remaining.front() == "all") for (const auto& [name, text] : license_map)
				std::cout << "\n=== " << name << " ===\n" << text << "\n";
			else for(std::string key: remaining) {
				auto it = license_map.find(key);
				if (it != license_map.end())
					std::cout << "\n=== " << it->first << " ===\n" << it->second << "\n";
				else {
					std::cout << "Unknown license: " << key << "\n";
					std::cout << "Valid licenses:\n";
					for (const auto& [name, _] : license_map)
						std::cerr << "  - " << name << "\n";
					return 1;				
				}
			}
		} else {
			std::cout<< "Please choose one of the following: ";
			for (auto it = license_map.begin(); it != license_map.end(); ++it) {
				std::cout << it->first;
				if (std::next(it) != license_map.end())
					std::cout << ", ";
			}
			std::cout << std::endl << std::endl;
		}
		return 0;
	}

	std::signal(SIGINT, signal_handler); std::signal(SIGTERM, signal_handler);
	std::signal(SIGHUP, signal_handler); std::signal(SIGQUIT, signal_handler);
	
	h5::mute();
    try {
		std::tie(interval, time_range, date_range, output_path_or_url, rts_path, instruments_path, trading_days_path, compression_level, convert, benchmark_format) = std::make_tuple(
			program.get<std::string>("--time-interval"), program.get<std::string>("--time-range"), program.get<std::string>("--date-range"),
			program.get<std::string>("--output"),
			program.get<std::string>("--rts-path"), program.get<std::string>("--instruments-path"), program.get<std::string>("trading-days-path"),
			program.get<unsigned>("--gzip"), program.get<std::string>("--convert"), program.get<std::string>("--benchmark-format"));

		bool is_irts_enabled = (convert == "all" || convert =="irts"),
			is_rts_enabled = (convert == "all" || convert =="rts");
		std::string dispatch = file::detect_format(output_path_or_url);
		std::vector<std::string> files = utils::resolve_input_paths(program.get<std::vector<std::string>>("remaining"));
		
		if(files.size()) {
			cout << "\033[1m[iex2h5]\033[0m Converting " << files.size()
			<< " file" << (files.size() > 1 ? "s" : "") 
			<< " using backend: " << dispatch << " — using 1 thread — © Varga Consulting, 2017–2025\n"
			<< "\033[1m[iex2h5]\033[0m Visit \033[4mhttps://vargaconsulting.github.io/iex2h5/\033[0m — Star it, Share it, Support Open Tools ⭐️\n";
			std::pair<std::string,std::string> time = utils::parse::time_interval(time_range),
				date = utils::parse::date_interval(date_range);
			
			global::state::total_input = utils::path_size(program.get<std::vector<std::string>>("remaining"));
			global::state::total_output_before = utils::path_size(output_path_or_url);

			std::map<std::string, std::function<void()>> execute {
				{"hdf5", io::create<io::hdf5::consumer_t>(files, date, time, interval, output_path_or_url, rts_path, instruments_path, trading_days_path, is_irts_enabled, is_rts_enabled, compression_level)},
				{"csv", io::create<io::csv::consumer_t>(files, date, time, interval, output_path_or_url, instruments_path, trading_days_path, is_irts_enabled, is_rts_enabled)},				
				{"json", io::create<io::json::consumer_t>(files, date, time, interval, output_path_or_url, instruments_path, trading_days_path, is_irts_enabled, is_rts_enabled)},			
				{"redis", io::create<io::redis::consumer_t>(files, date, time, interval, output_path_or_url, instruments_path, trading_days_path, is_irts_enabled, is_rts_enabled)}
#ifdef IEX2H5_HAVE_PARQUET
				,{"parquet", io::create<io::parquet_arrow::consumer_t>(files, date, time, interval, output_path_or_url, instruments_path, trading_days_path, is_irts_enabled, is_rts_enabled)}
#endif
			};


			if(!execute.contains(dispatch))
				std::cerr << "[iex2h5] error: unknown dispatch backend: " << dispatch << std::endl;
			else try {
				using gs = global::state;
				execute[dispatch]();
				gs::total_output_after = utils::path_size(output_path_or_url);
				uint64_t total_output_difference = gs::total_output_after - gs::total_output_before;
				std::string benchmark_line = benchmark_format != "csv" ?
					fmt::format("benchmark: {} events in {}ms  {:.1f} Mticks/s, {:.6f} µs/tick latency, {} input converted into {} output",
						gs::event_count, gs::duration, gs::event_rate / 1e6, gs::event_latency / 1e3, utils::human_readable(gs::total_input), utils::human_readable(total_output_difference)) 
					: fmt::format("{},{},{},{},{},{},{},{},{},{},{},{},{}",
						convert, dispatch, gs::instrument_count, gs::rts_count, gs::event_count, gs::duration, gs::event_rate, gs::event_latency, gs::total_input,
						total_output_difference, compression_level, time_range, date_range);
				cerr << benchmark_line << endl;

				cout << "\033[1m[iex2h5]\033[0m Conversion complete — all files processed successfully \n" << iex_attribution << std::endl;
			} catch (const global::shutdown_exception& ex){
				cout << "\n\033[1m[iex2h5]\033[0m Conversion interrupted \n" << iex_attribution << std::endl;
			}
		}
	} catch( const std::exception& err ) {
		cerr << err.what() << endl;
        cerr << program << endl;
		return 1;
    }
	h5::unmute();
    return 0;
}
