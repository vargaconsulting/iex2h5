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

#include <iex/protocol>
#include <iex/producer>
#include <iex/consumer>
#include <h5cpp/all>

#include <glog/logging.h>
#include <cstdio>
#include <date/tz.h>
#include <iostream>
#include <algorithm>

namespace ch = std::chrono;

void save_assets( const std::string input, const std::string output,
		  const std::string tradingdays_path, const std::string assets_path,
		  const std::string day_begin, const std::string day_end,  unsigned long interval ){

	using Consumer = iex::AssetConsumer<ch::system_clock>;
	using Producer = iex::PcapProducer<Consumer>;
	using duration = typename Consumer::duration;
	using namespace std::chrono;

	auto start_ = utils::string2duration<duration>(day_begin,"%H:%M:%S");
	auto stop_  = utils::string2duration<duration>(day_end, "%H:%M:%S");
	auto interval_ = duration_cast<duration>( seconds(interval));

	Producer producer( stdin, interval_ );
	Consumer consumer(output, tradingdays_path, assets_path);

	io::execute( producer, consumer, start_, interval_,  stop_ );
}



