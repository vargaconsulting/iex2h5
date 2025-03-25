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
 *   Copyright © <2018-2020> Varga Consulting, Toronto, On     info@vargaconsulting.ca
 *   _________________________________________________________________________________
 */

#ifndef IEX_DEEPS_H
#define IEX_DEEPS_H
namespace iex { namespace deeps {
	using header = iex::protocol::header;
}}

namespace iex::deeps::v105 {
	/** The System Event Message is used to indicate events that apply to the market or the data feed.
	 * There will be a single message disseminated per channel for each System Event type within a given trading session
	 */
	   	// 'S' 0x53 10 bytes
		// 'O' - start of msg,    'S' start_sys_hours,     'R' - start regular market 
		// 'C' - end of messages, 'E' - end system hours,  'M' - end regular market	
		// The time of the update event as set by the IEX Trading System lo
	using system = iex::protocol::header;
	//static_assert( sizeof(system) + sizeof(header) == 10, "not aligned to byte!!!");
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
	} __attribute__((packed));

	struct luld {
		uint8_t na 	   : 1,
				tier_1 : 1,
				tier_2 : 1;
	}; // 1 byte
		// 'H' - halted across US equity, 
		// 'O' - halt released into Order Acceptance Period on IEX
		// 'P' - Trading paused and Order Acceptance Period on IEX  
		// 'T' - Trading on IEX
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

	struct operational_halt_status { // 'O' 0x4f 18 bytes
		int zero[0];
	};

	struct short_sale_price_test_status{ // 'P' 0x50 19 bytes  
		uint8_t detail;
	}__attribute__((packed));

	struct security_event{ // 'E' 0x45 18 bytes
		int zero[0]; // this may not work: zero length class hack
	}__attribute__((packed));

	struct price_level_update{ // '8' 0x38, '5' 0x35 30 bytes
		uint32_t size;
		uint64_t price;
	}__attribute__((packed));

	struct trade_report{ // 'T' 0x54 38 bytes
		uint32_t size;
		uint64_t price;
		uint64_t id;
	}__attribute__((packed));

	struct official_price{ // 'X' 0x58 26 bytes
		uint64_t price;
	}__attribute__((packed));

	struct trade_break{ // 'B' 0x42 38 bytes
		uint64_t price;
	}__attribute__((packed));

	struct auction_information { //'A' 80 bytes
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

	struct message {
		iex::deeps::header hdr;
		union {
			security_directory sd;
			trading_status ts;
			operational_halt_status ohs;
			short_sale_price_test_status sts;
			security_event se;
			price_level_update plu;
			trade_report tr;
			official_price op;
			trade_break tb;
			auction_information ai;
		} __attribute__((packed));
	}__attribute__((packed));
}
#endif
