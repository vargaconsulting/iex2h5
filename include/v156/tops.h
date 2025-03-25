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
#ifndef IEX_TOPS156_H
#define IEX_TOPS156_H
namespace iex::tops {
	/**
	 * price: 8byte int fixed 4 digit decimal
	 * fields are little endian or intel order
	 * int 4byte
	 * long 8byte
	 */
	static constexpr int protocol_id = 0x8002;
	static constexpr int channel_id  = 1;

	using header = iex::protocol::header;
}

namespace iex::tops::v156 { // udpated May 09, 2017
		/** 42 byte length, in intel order
		 */
		struct quote_update { //'Q' (0x51) 42 bytes
			uint32_t bid_size; //!< bid 
			int64_t bid_price;//!< price
			int64_t ask_price;//!< price 
			uint32_t ask_size; //!< ask
		} __attribute__((packed));
		static_assert( sizeof(quote_update) + sizeof(header) == 42, "not aligned to byte!!!");

		struct trade_report { //'T' (ox54) 42 bytes 
			uint32_t size;
			int64_t price; 		//!< fixed last 4 digit is frational
			int64_t trade_id;   //!< iex generated id, may reference to trade break message
			int32_t reserved;
		} __attribute__((packed));
		using trade_break = trade_report;
		static_assert( sizeof(trade_report) + sizeof(header) == 42, "not aligned to byte!!!");
		static_assert( sizeof(trade_break)  + sizeof(header) == 42, "not aligned to byte!!!");

		struct quote_update_flag {
			uint8_t symbol_halt 	: 1,
					market_session  : 1;
		};
		struct trade_break_flag {
				uint8_t 
					intermarket_sweep 		: 1, //!< 0 - non intermarket sweep order, 1 - ISO
					extended_hours 			: 1,
					odd_lot 				: 1,
					trade_through_exempt 	: 1 ;
		};

	struct message {
		iex::tops::header hdr;
		union {
			trade_report tr;
			quote_update qu;
			trade_break  tb;
		}__attribute__((packed));
	}__attribute__((packed));
}

namespace iex::tops::v164 { // udpated Fab 27, 2018
	static constexpr int protocol_id = 0x8003;
	static constexpr int channel_id  = 1;
}
#endif
