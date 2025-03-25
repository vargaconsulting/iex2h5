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

#ifndef IEX_IRTS_CONSUMER_HPP
#define	IEX_IRTS_CONSUMER_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <io/interface>
#include <analytics/all>
#include <date/tz.h>
#include <glog/logging.h>
#include <algorithm>
#include <armadillo>
#include <h5cpp/all>

static inline std::string rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
	return s;
}

namespace iex {
	namespace ch = std::chrono;
	template <class Clock> struct IrtsConsumer :
		public io::Consumer<IrtsConsumer<Clock>, Clock> {
		using time_point = typename Clock::time_point;
		using duration = typename Clock::duration;
		using parent = typename io::Consumer<IrtsConsumer<Clock>, Clock>;

		IrtsConsumer( const std::string file_path, const std::string tradingdays_path )
			: file_path(file_path), tradingdays_path(tradingdays_path) {
		}
		void init( const std::vector<time_point>& days, const std::vector<std::string>& symbols, const std::vector<duration>& rts );

		void begin(uint64_t I, uint64_t S,  const std::vector<duration>& rts );
		void trade_report_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );
		void ask_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){};
		void bid_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){};
		void trade_break_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){};
		void heart_beat_impl( time_point time ){};
		void day_begin_impl( time_point day );
		void day_end_impl( time_point day );

		uint64_t slot;


		

	private:
		h5::fd_t fd;
		std::vector<std::vector<double>> time, trade;
		std::vector<std::string> symbols;
		std::vector<size_t> trade_count;
		const std::string file_path, tradingdays_path;
		time_point last_time, today;
	};
}


template <class Clock>
void iex::IrtsConsumer<Clock>::begin(uint64_t I, uint64_t S,  const std::vector<duration>& rts ){
	namespace an = analytics;
	trade.resize(I), time.resize(I); trade_count.resize(I);
	
}
template <class Clock>
void iex::IrtsConsumer<Clock>::init( const std::vector<time_point>& days, 
		const std::vector<std::string>& symbols, const std::vector<duration>& rts ){
	for(auto s: symbols)
		this->symbols.push_back(rtrim(s));
	parent::init(days, symbols, rts);
}

template <class Clock>
void iex::IrtsConsumer<Clock>::trade_report_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){
	this->time[stock].push_back(time.time_since_epoch().count());
	this->trade[stock].push_back(price);
	this->trade_count[stock] ++;
}


template <class Clock>
void iex::IrtsConsumer<Clock>::day_begin_impl( time_point day ){
	namespace an = analytics;
	using namespace date;
	using namespace std::chrono;

	std::string today = date::format("%F", floor<days>(day));
	LOG(INFO) << today;

	fd = h5::open(file_path, H5F_ACC_RDWR );
	an::zeros(trade_count);
}

template <class Clock>
void iex::IrtsConsumer<Clock>::day_end_impl( time_point day ){
	std::string today = date::format("%F", floor<days>(day));
	for(size_t i=0; i< symbols.size(); i++) {
		if( trade_count[i] > 1000)
			h5::write(fd, "irts/" + today + "/" + symbols[i] + ".trade", trade[i], h5::max_dims{H5S_UNLIMITED}, h5::chunk{1024} | h5::gzip{9} ),
			h5::write(fd, "irts/" + today + "/" + symbols[i] + ".time", time[i], h5::max_dims{H5S_UNLIMITED}, h5::chunk{1024} | h5::gzip{9} );
	}
	h5::write(fd, "trade_count.txt", trade_count);
}

#endif

