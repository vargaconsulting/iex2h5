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
#include <glog/logging.h>
#include <analytics/all>

#ifndef IO_EXECUTE_HPP
#define IO_EXECUTE_HPP
namespace io {
	template <typename P, typename Clock> using is_consumer  = typename std::is_base_of<Consumer<P,Clock>,P>;
	template <typename P, typename Clock> using is_producer  = typename std::is_base_of<Producer<P,Clock>,P>;

	/** wires up and executes a  CRTP Producer and CRTP Consumer class  \ingroup IO
	 * @see Consumer
	 * @see Producer
	 */ 
	template <typename Producer, typename Consumer, typename duration = typename Consumer::duration>
	void execute( Producer& producer, Consumer& consumer, duration start, duration interval, duration stop ) {
		using namespace analytics;
		static_assert( is_consumer<Consumer,typename Consumer::clock>::value,"Second argument is not a CRTP of Consumer!" );
		producer.run( consumer, start, stop );
	}
}
#endif
