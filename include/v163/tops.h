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
#ifndef IEX_TOPS163_H
#define IEX_TOPS163_H
namespace iex { namespace tops {
	using header = iex::protocol::header;
}}


namespace iex::tops::v163 {
/* ADMINISTRATIVE MESSAGE FORMATS */

		/** IEX disseminates a full pre-market spin of Security Directory Messages 
		 * for all IEX-listed securities. After the pre-market spin, IEX will use 
		 * the Security Directory Message to relay changes for an individual security.
		 */ 
		struct security_directory { // 'D' 0x44 31 bytes
			uint32_t lot_size; //!< Number of shares that represent a round lot
			/* The corporate action adjusted previous official closing price for the security (e.g., stock split, dividend, rights offering),
			where the decimal portion is zero filled on the right. The decimal point is implied by position and does not explicitly
			appear in the field. For example, 123400 = $12.34. When no corporate action has occurred, the Adjusted POC Price will
			be populated with the previous official close price. For new issues (e.g., an IPO), this field will be the issue price.
			*/
			int64_t poc_price; //!< Corporate action adjusted previous official closing price
			uint8_t luld; 	   //!< Indicates which Limit Up-Limit Down price band calculation parameter is to be used
		}__attribute__((packed)); // 31 bytes
		static_assert( sizeof(security_directory) + sizeof(header) == 31, "not aligned to byte!!!");

		/** 'H' - halted across US equity, 
		   'O' - halt released into Order Acceptance Period on IEX
		  'P' - Trading paused and Order Acceptance Period on IEX  
			 'T' - Trading on IEX
		*/
		struct trading_status{ // 'H' 0x48 22 bytes
			// Treading Halt reasons: 
			// 		T1: halt News Pending, IPO1: IPO/New Issue Not Yet Trading
			// 		IPOD: IPO/New Issue Deferred, MCB3: Market Wide Cicuit Breaker Level 3 - Breached
			// 		NA: Reason Not Available
			// Order Acceptance Period Reasons:
			// 		T2: Halt News Dissemination, IPO2: IPO/News Issue Order Acceptance Period
			// 		IPO3: IPO Pre-Launch Period
			// 		MCB1: Market Wide Curcuit Breaker Level 1 - breached
			// 		MCB2: Market Wide Curcuit Breaker Level 2 - breached
			uint8_t reason[4];
		}__attribute__((packed)); // 22  bytes
		static_assert( sizeof(trading_status) + sizeof(header) == 22, "not aligned to byte!!!");

		// 'O' IEX specific halt, 'N' - not operationally halted
		struct operational_halt_status { // 'O' 0x4f 18 bytes
			int arr[0];
		}__attribute__((packed));
		static_assert( sizeof(operational_halt_status) + sizeof(header) == 18, "zero size class (hack) didn't work: please find me in source code!!!");

		struct short_sale_price_test_status{ // 'P' 0x50 19 bytes  
			uint8_t detail;
		}__attribute__((packed));
		static_assert( sizeof(short_sale_price_test_status) + sizeof(header) == 19, "not aligned to byte!!!");

/* TRADING MESSAGE FORMATS */
		/** 42 byte length, in intel order
		 */
		struct quote_update { //'Q' (0x51) 42 bytes
			int32_t bid_size; //!< bid 
			int64_t bid_price;//!< price
			int64_t ask_price;//!< price 
			int32_t ask_size; //!< ask
		}__attribute__((packed));
		static_assert( sizeof(quote_update) + sizeof(header) == 42, "not aligned to byte!!!");

		struct trade_report { //'T' (ox54) 38 bytes 
			uint32_t size;
			int64_t price; 		//!< fixed last 4 digit is frational
			int64_t trade_id;   //!< iex generated id, may reference to trade break message
		}__attribute__((packed));
		static_assert( sizeof(trade_report) + sizeof(header) == 38, "not aligned to byte!!!");

		struct official_price{ // 'X' 0x58 26 bytes
			uint64_t price;
		}__attribute__((packed));
		static_assert( sizeof(official_price) + sizeof(header) == 26, "not aligned to byte!!!");

		using trade_break = trade_report;
		static_assert( sizeof(trade_break) + sizeof(header) == 38, "not aligned to byte!!!");

		struct auction_information { // 80 bytes
			uint32_t paired_shares;
			uint64_t reference_price;
			uint64_t indicative_clearing_price;
			uint32_t imbalance_shares;
			uint8_t imbalance_side;
			uint8_t extension_number;
			uint32_t scheduled_auciton_time;
			uint64_t auction_book_clearing_price;
			uint64_t collar_reference_price;
			uint64_t lower_auction_collar;
			uint64_t upper_auction_collar;
		}__attribute__((packed));
		static_assert( sizeof(auction_information) + sizeof(header) == 80, "not aligned to byte!!!");

		struct quote_update_flag{
			uint8_t symbol_halt 	: 1,
					market_session  : 1;
		};
		struct trade_break_flag{
				uint8_t 
					intermarket_sweep : 1, //!< 0 - non intermarket sweep order, 1 - ISO
					extended_hours : 1,
					odd_lot : 1,
					trade_through_exempt : 1 ;
		};

		struct message {
			iex::tops::header hdr;
			union {
				security_directory sd;
				trading_status ts;
				operational_halt_status ohs;
				short_sale_price_test_status sps;
				quote_update qu;
				trade_report tr;
				official_price op;
				trade_break tb;
				auction_information aui;
			}__attribute__((packed));
		}__attribute__((packed));
}
#endif
