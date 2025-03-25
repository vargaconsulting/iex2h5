/*
 *   ALL RIGHTS RESERVED.
 *   _________________________________________________________________________________
 *   NOTICE: All information contained  herein is, and remains the property  of  Varga
 *   Consulting and  its suppliers, if  any. The intellectual and  technical  concepts
 *   contained herein are proprietary to Varga Consulting and its suppliers and may be
 *   covered  by  Canadian and  Foreign Patents, patents in process, and are protected
 *   by  trade secret or copyright law. Dissemination of this information or reproduc-
 *   tion  of  this  material is strictly forbidden unless prior written permission is
 *   obtained from Varga Consulting.
 *
 *   Copyright © <2017-2025> Varga Consulting, Toronto, On     info@vargaconsulting.ca
 *   _________________________________________________________________________________
 */

//#include <mpi.h>
#include <vector>
#include <glog/logging.h>
#include <boost/program_options.hpp>
#include <gperftools/profiler.h>
#include <iostream>

using namespace std;
namespace po = boost::program_options;
// declarations from *.cpp files
void save_assets( const std::string input, const std::string output,
		  const std::string days_path, const std::string assets_path,
		  const std::string day_begin, const std::string day_end,  unsigned long interval );

void generate_irts( const std::string input, const std::string output,
		  const std::string days_path, const std::string assets_path,
		  const std::string day_begin, const std::string day_end,  unsigned long interval );

void generate_rts( const std::string  input, const std::string output,
		  const std::string days_path, const std::string assets_path,
		  const std::string day_begin, const std::string day_end,  unsigned long interval );

void generate_index( const std::string input, const std::string output,
		  const std::string days_path, const std::string assets_path,
		  const std::string day_begin, const std::string day_end,  unsigned long interval );



using command = void(const std::string, const std::string, const std::string, const std::string,
		  const std::string, const std::string,  unsigned long);

int main(int argc, char **argv) {

	std::string input,output,stream,rts,glog_dir,
			assets,days,day_begin,day_end,asset_idx,trading_days_idx, cmd;
	bool glog_stderr;
    unsigned int glog_minloglevel, time_interval, filter, chunk;
    po::options_description desc("Allowed options",120);
	// simple dispatch map:
	std::map<std::string,std::function<command>> dispatch;
	dispatch["assets"] 	= save_assets;
	dispatch["irts"] 	= generate_irts;
	dispatch["rts"] 	= generate_rts;
	dispatch["index"] 	= generate_index;

	desc.add_options()
			("time-interval", po::value<unsigned int>()->default_value(10), "temporal interval in seconds, irts stream is converted into")
			("start", po::value<std::string>()->default_value("14:30:00"), "lower bound on processing data stream ")
			("stop",  po::value<std::string>()->default_value("21:00:00"), "upper bound on processing stream\n")

			("input,i", po::value<string>(), "packet capture file or when left empty: stdin")
            ("output,o", po::value<string>()->default_value("./iex.h5"), "output hdf5 file")
            ("rts,r", po::value<string>()->default_value("/time.txt"), "hdf5-group/directory for regular time interval datasets")

			("asset-path", po::value<string>()->default_value("/instruments.txt"), 
			 							"path to HDF5 index dataset for listed [symbols|assets|financial] instruments")
            ("trading-days-path", po::value<string>()->default_value("/trading_days.txt"),
			 		"path to HDF5 dataset containing the list of trading days")

			("gzip,g", po::value<unsigned int>()->default_value(0), "0-9 0 for no compression, 9 for highest")
            ("chunk,c", po::value<unsigned int>()->default_value(1), "number of days in blocks/hdf5-chunks, 0 no-chunks \n\n")

            ("command", po::value<string>(),
			 		"irts    - saves captured events as irts stream\n"
			 		"rts     - converts irts to rts\n"
					"assets  - retrieves symbols from irts/stream\n"
					"index   - creates trading days\n"
					"\n")

			("glog-dir", po::value<string>()->default_value("./"),"glog output directory") 
            ("glog-stderr", po::value<bool>()->default_value(true),"glog output to stderr if true") 
            ("glog-minloglevel", po::value<unsigned int>()->default_value(0),"glog log level:  INFO=0 WARNING=1 ERROR=2 FATAL=3\n")
            ("help,h", "produce help message")
            ;
    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
			cout << "\033[1m" "IEX2H5 converts IEX TOPS Datasets to HDF5 Format" "\033[0m" << std::endl << std::endl;
			cout << "iex2h5 is a specialized tool for importing IEX TOPS datasets into the HDF5 data format," << std::endl;
			cout << "enabling efficient  storage and analysis of large  financial datasets. HDF5 is a widely"  << std::endl;
			cout << "used file format for handling large, complex, and hierarchical data, supported by major" << std::endl;
			cout << "programming languages including Julia, Python, MATLAB, C, C++, and Node.js."  << std::endl;
			cout << std::endl;	
			cout << "This application allows users to convert captured packet data streams (e.g., DEEP/TOPS)"<< std::endl;
			cout << "into structured HDF5 datasets for advanced analytics and seamless integration into"<< std::endl;
			cout << "scientific, engineering, and financial workflows." << std::endl;
			cout << desc << std::endl;

            cout <<"\033[1m" "example:" "\033[0m" <<endl;
            cout <<"   "<< argv[0] <<endl;
			cout << "Copyright © <2017-2025> Varga Consulting, Toronto, ON, info@vargaconsulting.ca" << std::endl << std::endl;
            return 0;
        }

        output = vm["output"].as<string>();
        rts = vm["rts"].as<string>();
        filter = vm["gzip"].as<unsigned int>(); chunk  = vm["chunk"].as<unsigned int>();

        glog_stderr = vm["glog-stderr"].as<bool>(); glog_dir = vm["glog-dir"].as<string>();
        glog_minloglevel = vm["glog-minloglevel"].as<unsigned int>();
		cmd = vm["command"].as<string>();
        time_interval = vm["time-interval"].as<unsigned int>();
		days 	= vm["trading-days-path"].as<std::string>();  	assets 	= vm["asset-path"].as<std::string>();
		day_begin 	= vm["start"].as<std::string>();  		day_end = vm["stop"].as<std::string>();

		FLAGS_log_dir = glog_dir; // initialize before InitGoogleLogging( ... ) is called

        google::InitGoogleLogging( argv[0] );

        FLAGS_logtostderr = glog_stderr;
        FLAGS_logbufsecs = 1;
        FLAGS_minloglevel = glog_minloglevel;
        FLAGS_alsologtostderr = glog_stderr;
#ifdef DEBUG
		ProfilerStart( (std::string(argv[0]) + std::string(".prof")).data() );
#endif
		if( !vm.count("input") ){
			try {
				dispatch[cmd](input, output, days, assets, day_begin, day_end, time_interval );
			} catch( const std::runtime_error& e ){
				LOG(INFO) <<"ERROR: " << e.what();
			}
		} else {
			input  = vm["input"].as<string>();
			dispatch[cmd](input, output, days, assets, day_begin, day_end, time_interval );
			DLOG(INFO) << input;
		}
#ifdef DEBUG
		ProfilerStop();
#endif
	} catch( ... ){
        cout << endl << "Error parsing arguments!!! "<<endl<<endl;
        cout << desc <<endl<<endl;
    }
    return 0;
}

