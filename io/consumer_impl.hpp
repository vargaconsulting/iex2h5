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

#ifndef IO_CONSUMER_IMP_HPP
#define IO_CONSUMER_IMP_HPP
#include <limits>

template <typename Derived, typename Clock>
void io::Consumer<Derived,Clock>::init( const std::vector<time_point>& trading_days,
		const std::vector<std::string>& symbols, const std::vector<duration>& rts ){
	// fill map with requested symbols
	uint64_t seq=0;
	for( std::string sym:symbols )
		map.emplace( std::make_pair(* (uint64_t*) sym.data(), seq++));
	this->symbols = symbols;
	uint64_t I = symbols.size();
	uint64_t S = rts.size();
	max_stock = I;
	static_cast<Derived*>(this)->begin(I,S, rts);
}

/* symbol translation is NOT requested */
template <typename Derived, typename Clock>
template <typename U, typename io::with_symbols<U>::type>
void io::Consumer<Derived,Clock>::trade_report(time_point time,  uint64_t symbol, float price, uint64_t size, uint8_t flag ){
	static_cast<Derived*>(this)->trade_report_impl(time,symbol,price,size,flag);
}
template <typename Derived, typename Clock>
template <typename U, typename io::with_symbols<U>::type>
void io::Consumer<Derived,Clock>::ask(time_point time,  uint64_t symbol, float price, uint64_t size, uint8_t flag ){
	static_cast<Derived*>(this)->ask_impl(time,symbol,price,size,flag);
}
template <typename Derived, typename Clock>
template <typename U, typename io::with_symbols<U>::type>
void io::Consumer<Derived,Clock>::bid(time_point time,  uint64_t symbol, float price, uint64_t size, uint8_t flag ){
	static_cast<Derived*>(this)->bid_impl(time,symbol,price,size,flag);
}
template <typename Derived, typename Clock>
template <typename U, typename io::with_symbols<U>::type>
void io::Consumer<Derived,Clock>::trade_break(time_point time,  uint64_t symbol, float price, uint64_t size, uint8_t flag ){
	static_cast<Derived*>(this)->trade_break_impl(time,symbol,price,size,flag);
}

/* alternatives whether symbol translation is requested */
template <typename Derived, typename Clock>
template <typename U, typename io::without_symbols<U>::type>
void io::Consumer<Derived,Clock>::trade_report(time_point time,  uint64_t symbol, float price, uint64_t size, uint8_t flag ){
	uint64_t id = this->symbol2id( symbol );
	if(id < max_stock)	static_cast<Derived*>(this)->trade_report_impl(time,id,price,size,flag);
}
template <typename Derived, typename Clock>
template <typename U, typename io::without_symbols<U>::type>
void io::Consumer<Derived,Clock>::ask(time_point time,  uint64_t symbol, float price, uint64_t size, uint8_t flag ){
	uint64_t id = this->symbol2id( symbol );
	if(id < max_stock) static_cast<Derived*>(this)->ask_impl(time,id,price,size,flag);
}
template <typename Derived, typename Clock>
template <typename U, typename io::without_symbols<U>::type>
void io::Consumer<Derived,Clock>::bid(time_point time,  uint64_t symbol, float price, uint64_t size, uint8_t flag ){
	uint64_t id = this->symbol2id( symbol );
	if( id < max_stock ) static_cast<Derived*>(this)->bid_impl(time,id,price,size,flag);
}
template <typename Derived, typename Clock>
template <typename U, typename io::without_symbols<U>::type>
void io::Consumer<Derived,Clock>::trade_break(time_point time,  uint64_t symbol, float price, uint64_t size, uint8_t flag ){
	uint64_t id = this->symbol2id( symbol );
	if( id < max_stock )	static_cast<Derived*>(this)->trade_break_impl(time,id,price,size,flag);
}

template <typename Derived, typename Clock>
void io::Consumer<Derived,Clock>::heart_beat( time_point time ){
	static_cast<Derived*>(this)->heart_beat_impl( time );
}

template <typename Derived, typename Clock>
void io::Consumer<Derived,Clock>::day_end( time_point day ){
	static_cast<Derived*>(this)->day_end_impl( day );
}

template <typename Derived, typename Clock>
void io::Consumer<Derived,Clock>::day_begin( time_point day ){
	static_cast<Derived*>(this)->day_begin_impl( day );
}

template <typename Derived, typename Clock>
uint64_t io::Consumer<Derived,Clock>::symbol2id(uint64_t symbol){
	const auto& it = map.find( symbol );
	return it != map.end() ? it->second : IEX_MAX_SYMBOLS;
}
#endif
