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

#include <chrono>
#include <date/tz.h>
#include <cstddef>

#ifndef IO_PRODUCER_HPP
#define IO_PRODUCER_HPP
namespace io {
	/** Base class to consume High Frequency Trading events, must be used in CRTP fashion \ingroup IO
	 */
	template <typename Derived, typename Consumer> struct Producer {
		using type  = Derived; 							//!< CRTP derived type
		using clock = typename Consumer::clock;			//!< std::chrono clock used by T derived
		using rep   = typename clock::rep; 				//!< integral type, the representation of clock
		using duration   = typename clock::duration; 	//!< used to define the span between single ticks
		using time_point = typename clock::time_point;  //!< how time is expressed in producer

		/** feeds market events  into a Consumer object  
		 */
		void run( Consumer& ref, duration start, duration stop );
		/** regular interval callback defined by templated clock */
		void heart_beat( time_point tp );
		//data flow control methods
		void begin( time_point tp  );
		void end( time_point tp  );
		/** an event to report official start time of given market and the set of instruments traded */ 
		void day_end( time_point day );
		/** fired to notify official trading hours are over on market */
		void day_begin( time_point day );

		void trade_report(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );
		void ask(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );
		void bid(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );
		void trade_break(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );

		duration start,stop,heart_beat_interval;
		private:
			Consumer* consumer;
			Producer(){};
			friend Derived;
	};
}
#endif
