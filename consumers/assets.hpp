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

#ifndef IEX_ASSET_CONSUMER_HPP
#define	IEX_ASSET_CONSUMER_HPP

#include <string>
#include <vector>
#include <io/interface>
#include <date/tz.h>
#include <glog/logging.h>
#include <h5cpp/all>
#include <map>

using namespace std;
using namespace date;

namespace iex {
	class WithSymbols{};
	namespace ch = std::chrono;
	template <class Clock> struct AssetConsumer :
		public io::Consumer<AssetConsumer<Clock>, Clock>, io::WithSymbols {
		using time_point = typename Clock::time_point;
		using duration = typename Clock::duration;

		AssetConsumer( const std::string file_path, const std::string tradingdays_path, const std::string asset_path)
			: file_path(file_path), asset_path(asset_path), tradingdays_path(tradingdays_path) {
		}

		void begin(uint64_t I, uint64_t S,  const std::vector<duration>& rts ){};
		void trade_report_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );
		void ask_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){};
		void bid_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){};
		void trade_break_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){};
		void heart_beat_impl( time_point time ){};
		void day_begin_impl( time_point day );
		void day_end_impl( time_point day );

		const std::string file_path, asset_path, tradingdays_path;
		int count;
		std::map<std::string,int> map;
		void insert(uint64_t stock ){
			char *c = (char*) &stock;
			std::string key(c,c+8);
			map[key] = count++;
		}
	};
}

template <class Clock>
void iex::AssetConsumer<Clock>::trade_report_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){
	insert(stock);
}

template <class Clock>
void iex::AssetConsumer<Clock>::day_begin_impl( time_point day ){
	count = 0;
}
template <class Clock>
void iex::AssetConsumer<Clock>::day_end_impl( time_point day ){
	LOG(INFO) << map.size();
	std::vector<std::string> assets;
	for(auto i:map ) assets.push_back( i.first );
	std::sort(assets.begin(), assets.end());
	auto fd = h5::create(file_path, H5F_ACC_TRUNC );
	h5::write(fd, asset_path, assets);
}
#endif

