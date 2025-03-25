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

#ifndef IEX_TRANSPORT_PRODUCER_HPP
#define	IEX_TRANSPORT_PRODUCER_HPP

#include <io/interface>
#include <pcap/pcap.h>
#include <glog/logging.h>
#include <string>
#include <cstddef>
#include <date/tz.h>

namespace iex {
	/** @ingroup IEX
	 * extracts data from pcap stream, then after disassembling packets delegates to io::Consumer */
	template <typename Consumer> struct Transport :
	public io::Producer<Transport<Consumer>,Consumer> {
		using Block = iex::protocol::block;
		using time_point = typename Consumer::clock::time_point;
		using duration = typename Consumer::clock::duration;

		Transport();
		/** IEX transport header parser to recover message blocks
		 * which encapsulate deeps and tops messages
		 */
		void transport_handler( const iex::transport::header* segment );
		virtual void run_impl() = 0;

		std::vector<std::string> symbols;
		private:
			void tops_v156(  const iex::tops::v156::message  * msg );
			void tops_v163(  const iex::tops::v163::message  * msg );
			void deeps_v105( const iex::deeps::v105::message * msg );
			long count;
			bool is_market_opened,is_market_closed,is_first_beat;
			time_point today;
			time_point last_time;
	};
}
#include "itransport_v125.hpp"
#include "itops_v156.hpp"
#include "itops_v163.hpp"
#include "ideeps_v105.hpp"
#endif

