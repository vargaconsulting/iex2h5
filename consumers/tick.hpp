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


#ifndef IEX_TICK_HPP
#define	IEX_TICK_HPP

namespace iex {
	typedef struct Tick {
		long long time;
		uint32_t    asset;
		float  ask_price;
		float  trade_price;
		float  bid_price;
		unsigned long ask_size;
		unsigned long trade_size;
		unsigned long bid_size;
	} tick_type;
}

namespace h5{
	// specialize template and create a compound data type
    template<> hid_t inline register_struct<iex::tick_type>(){
			hid_t type = H5Tcreate(H5T_COMPOUND, sizeof (iex::tick_type));
			// layout
			H5Tinsert(type, "time", 		HOFFSET(iex::tick_type, time), 			H5T_NATIVE_LLONG);
			H5Tinsert(type, "asset", 		HOFFSET(iex::tick_type, asset), 		H5T_NATIVE_INT);
			H5Tinsert(type, "ask_price", 	HOFFSET(iex::tick_type, ask_price), 	H5T_NATIVE_FLOAT);
			H5Tinsert(type, "trade_price", 	HOFFSET(iex::tick_type, trade_price), 	H5T_NATIVE_FLOAT);
			H5Tinsert(type, "bid_price", 	HOFFSET(iex::tick_type, bid_price), 	H5T_NATIVE_FLOAT);
			H5Tinsert(type, "ask_size", 	HOFFSET(iex::tick_type, ask_size), 		H5T_NATIVE_ULONG);
			H5Tinsert(type, "trade_size", 	HOFFSET(iex::tick_type, trade_size), 	H5T_NATIVE_ULONG);
			H5Tinsert(type, "bid_size", 	HOFFSET(iex::tick_type, bid_size), 		H5T_NATIVE_ULONG);
            return type;
	};
}
H5CPP_REGISTER_STRUCT( iex::tick_type );
#endif
