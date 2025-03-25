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

#ifndef IEX_EXAMPLE_CONSUMER_HPP
#define	IEX_EXAMPLE_CONSUMER_HPP

#include <string>
#include <vector>
#include <io/interface>
#include <date/tz.h>
#include <glog/logging.h>
#include <unordered_map>

using namespace std;
using namespace date;

namespace iex {
	namespace ch = std::chrono;
	template <class Clock> struct IOStreamConsumer :
		public io::Consumer<IOStreamConsumer<Clock>, Clock> {
		using time_point = typename Clock::time_point;
		using duration = typename Clock::duration;

		void begin(uint64_t I, uint64_t S,  const std::vector<duration>& rts ){};
		void trade_report_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );
		void ask_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );
		void bid_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );
		void trade_break_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );
		void heart_beat_impl( time_point time );
		void day_begin_impl( time_point day );
		void day_end_impl( time_point day );

		long count;
		uint64_t seq;
		std::unordered_map<uint64_t,uint64_t>map;
	};
}

template <class Clock>
void iex::IOStreamConsumer<Clock>::trade_report_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){
}
template <class Clock>
void iex::IOStreamConsumer<Clock>::ask_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){
}
template <class Clock>
void iex::IOStreamConsumer<Clock>::bid_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){
}
template <class Clock>
void iex::IOStreamConsumer<Clock>::trade_break_impl(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){
}
template <class Clock>
void iex::IOStreamConsumer<Clock>::heart_beat_impl( time_point tp ){
	using namespace std;
	using namespace date;
}
template <class Clock>
void iex::IOStreamConsumer<Clock>::day_begin_impl( time_point day ){
}
template <class Clock>
void iex::IOStreamConsumer<Clock>::day_end_impl( time_point day ){
}
#endif

