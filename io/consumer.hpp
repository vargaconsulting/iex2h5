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
#include <unordered_map>

#include <algorithm>
#include <iostream>
#include <type_traits>

#ifndef IO_CONSUMER_HPP
#define IO_CONSUMER_HPP
namespace io {
	class WithSymbols{};
	template<class U> using    with_symbols = std::enable_if<std::is_base_of<WithSymbols,U>::value, int>;
	template<class U> using without_symbols = std::enable_if<!std::is_base_of<WithSymbols,U>::value, int>;

	/** Base class to consume High Frequency Trading events, ust be used in CRTP fashion \ingroup IO
	 * @see execute
	 * @ingroup IO
	   		*/ 
	template <typename Derived, typename Clock>
	   	struct Consumer {

		using type  = Derived; 							//!< \brief CRTP derived type
		using clock = Clock; 							//!< std::chrono clock used by T derived
		using rep = typename Clock::rep; 				//!< integral type, the representation of clock
		using duration = typename Clock::duration; 		//!< used to define the span between single ticks
		using time_point = typename Clock::time_point;  //!< how time is expressed in consumer

		void init( const std::vector<time_point>& days, const std::vector<std::string>& symbols, const std::vector<duration>& rts );
		/** regular interval callback defined by templated clock  
		 */
		void heart_beat( time_point tp );
		//data flow control methods
		/** an event to report official start time of given market and the set of instruments traded */ 
		void day_end( time_point day );
		/** fired to notify official trading hours are over on market */
		void day_begin( time_point day );

		template <typename U = Derived, typename with_symbols<U>::type  = true>
		void trade_report(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );
		template <typename U = Derived, typename with_symbols<U>::type  = true>
		void ask(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );
		template <typename U = Derived, typename with_symbols<U>::type  = true>
		void bid(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );
		template <typename U = Derived, typename with_symbols<U>::type  = true>
		void trade_break(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );

		template <typename U = Derived, typename without_symbols<U>::type  = true>
		void trade_report(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );
		template <typename U = Derived, typename without_symbols<U>::type  = true>
		void ask(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );
		template <typename U = Derived, typename without_symbols<U>::type  = true>
		void bid(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );
		template <typename U = Derived, typename without_symbols<U>::type  = true>
		void trade_break(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag );

		std::unordered_map<uint64_t,uint64_t>map;
		std::vector<std::string> symbols;
		uint64_t symbol2id(uint64_t symbol);
	private:
		Consumer(){};
		friend Derived;
		uint64_t max_stock;
	};
}
#endif
