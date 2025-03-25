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

#ifndef IEX_RTS_CONSUMER_HPP
#define	IEX_RTS_CONSUMER_HPP

#include <string>
#include <vector>
#include <io/interface>
#include <analytics/all>
#include <date/tz.h>
#include <glog/logging.h>
#include <algorithm>
#include <armadillo>
#include <h5cpp/all>

namespace iex {
	namespace ch = std::chrono;
	template <class Clock> struct RtsConsumer :
		public io::Consumer<RtsConsumer<Clock>, Clock> {
			using time_point = typename Clock::time_point;
			using duration = typename Clock::duration;

		RtsConsumer( const std::string file_path, const std::string tradingdays_path)
			: file_path(file_path), tradingdays_path(tradingdays_path) {
		}
		void begin(uint64_t I, uint64_t S,  const std::vector<duration>& rts);
		void trade_report_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag);
		void ask_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag);
		void bid_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag);
		void trade_break_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag);
		void heart_beat_impl( time_point time );
		void day_begin_impl( time_point day );
		void day_end_impl( time_point day );

		uint64_t slot, max_slot;

	private:
		h5::fd_t fd;

		const std::string file_path, tradingdays_path;

		time_point last_time, today;
		arma::fmat h5_bid, h5_ask, h5_trade;
		arma::umat h5_bid_volume, h5_ask_volume, h5_trade_volume;
		arma::uvec trade_size, trade_count, event_count;
		arma::fvec avg_trade_count, avg_spread, day_high, day_low, day_close, day_open;
		std::vector<std::string> rts, start, stop;
		analytics::EMAFilter<Clock> fbid, fask, ftrade;
	};
}

/**
Parent `consumer_impl.hpp` parses 'time.txt' and 'instrument.txt' then 
generates the following:
I   - number of instruments
S   - time slot size, or regular intervals intra-day
rts - intra day time intervals in durations 
*/
template <class Clock>
void iex::RtsConsumer<Clock>::begin(uint64_t I, uint64_t S,  const std::vector<duration>& rts){
	namespace an = analytics;
	fd = h5::open(file_path, H5F_ACC_RDWR );
	this->max_slot = S;
	an::resize(I,S, h5_bid,h5_ask,h5_trade,  h5_bid_volume, h5_ask_volume, h5_trade_volume);
	an::resize(I,
		start, stop, 
		event_count, trade_size, trade_count, fbid, fask, ftrade,
		avg_trade_count, avg_spread, day_high, day_low, day_close, day_open);

	LOG(INFO) << I << "x" << S <<" " << h5_ask.n_rows <<"x"<<h5_ask.n_cols;
}

/**
maintain trade related statistics
*/
template <class Clock>
void iex::RtsConsumer<Clock>::trade_report_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag){
	if( slot >= max_slot ) return; //TODO: remove later
	
	h5_trade_volume(stock, slot) += size;
	trade_size[stock] += size;
 	trade_count[stock] ++;
	event_count[stock]++;
	ftrade(time, stock, price, size);
}

template <class Clock>
void iex::RtsConsumer<Clock>::ask_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag){
	if( slot >= max_slot ) return; //TODO: remove later
	h5_ask_volume(stock, slot) = size;
	fask(time, stock, price, size);
	event_count[stock]++;
}
template <class Clock>
void iex::RtsConsumer<Clock>::bid_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag){
	if( slot >= max_slot ) return; //TODO: remove later
	h5_bid_volume(stock, slot) = size;
	fbid(time, stock, price, size);
	event_count[stock]++;
}
template <class Clock>
void iex::RtsConsumer<Clock>::trade_break_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag){
}
template <class Clock>
void iex::RtsConsumer<Clock>::heart_beat_impl( time_point tp ){
	using namespace std;
	using namespace date;
	// direct memory access to matrix columns
	auto ask   =  h5_ask.unsafe_col(slot);
	auto trade =  h5_trade.unsafe_col(slot);
	auto bid   =  h5_bid.unsafe_col(slot);

	auto tp_ = date::format("%H:%M:%S", date::floor<chrono::seconds>(tp));
	rts.push_back( tp_ );
	fask.predict( ask ); ftrade.predict( trade ); fbid.predict( bid );

	for(uint64_t i = 0; i < trade.n_rows; i++){
		// trade is the estimate of the value of an instrument, if no trade happens
		// we split the spread
		if(trade[i] == 0 ) {
			if( ask[i] > 0 && bid[i] > 0 )
				trade[i] = bid[i] + 0.5 * (ask[i] - bid[i]);
		}

		if(start[i].length() == 0 && trade[i] > 0) 
			start[i] = tp_;
		else if(trade[i] > 0) stop[i] = tp_; 
	}		
	slot++;
}

/**
reset data structures when rolling over to a new trading day
*/
template <class Clock>
void iex::RtsConsumer<Clock>::day_begin_impl( time_point day ){
	namespace an = analytics;

	//TODO: initialize filters with last known price
	an::zeros(
		fbid, fask, ftrade, // filters
		h5_ask, h5_trade, h5_bid,  h5_bid_volume, h5_ask_volume, h5_trade_volume,
		trade_count, event_count, trade_size, avg_trade_count, avg_spread, day_high, day_low, day_close, slot);
}

template <class Clock>
void iex::RtsConsumer<Clock>::day_end_impl( time_point day ){
	namespace an = analytics;
	using namespace date;
	using namespace std::chrono;
	std::string today = date::format("%F", floor<days>(day));
	// RTS
	for( int i=0; i<avg_trade_count.size(); i++){
		avg_trade_count[i] = trade_count[i] / static_cast<float>( slot );
		// not traded assets/instruments have no `time` entries
		// setting them to `max` is sensible, as it spans 0 length
		if(start[i].empty()) start[i] = rts.back();
		if(stop[i].empty()) stop[i] = rts.back(); 
	}

	an::round<VALUE_PRECISION>(h5_ask, h5_trade, h5_bid, avg_trade_count);
	an::zeros2nans(h5_ask, h5_trade, h5_bid);

	h5::write(fd,"/rts/ask/"	+ today, h5_ask);
	h5::write(fd,"/rts/bid/"	+ today, h5_bid);
	h5::write(fd,"/rts/trade/"  + today, h5_trade);
	h5::write(fd,"/rts/volume/" + today, h5_trade_volume);

	h5::write(fd,"/stats/" + today + "/avg_trade_count", this->avg_trade_count);
	h5::write(fd,"/stats/" + today + "/trade_count", this->trade_count);
	h5::write(fd,"/stats/" + today + "/first_trade", this->start);
	h5::write(fd,"/stats/" + today + "/last_trade", this->stop);

	h5::write(fd,"/time.txt", this->rts);
	std::cout << today << std::endl;
}
#endif

