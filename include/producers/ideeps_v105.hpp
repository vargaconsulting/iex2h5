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

#ifndef IEX_DEEPS105_IMPL_HPP
#define	IEX_DEEPS105_IMPL_HPP

template <typename T>
void iex::Transport<T>::deeps_v105( const iex::deeps::v105::message* msg  ){

	using namespace deeps;
	time_point tp = time_point( duration( msg->hdr.time ));
	const v105::trade_report* tr = &msg->tr;
	const v105::trade_break*  tb = &msg->tb;

	// frequent: H,T,P,8   none: D,X,B  rare: O,E,A 
	switch( msg->hdr.type ){
		case '8': // bid: price level update buy size or bids
			this->bid(tp, msg->hdr.symbol, 1e-4*tr->price, tr->size, msg->hdr.flag );
			break;
		case '5': // ask: price level update or sell side or offer 
			this->ask(tp, msg->hdr.symbol, 1e-4*tr->price, tr->size, msg->hdr.flag );
			break;
		case 'T': // trade report
			this->trade_report(tp, msg->hdr.symbol, 1e-4*tr->price, tr->size, msg->hdr.flag );
			break;
		case 'B': // trade break
			this->trade_break(tp, msg->hdr.symbol, tb->price, 0,  msg->hdr.flag );
			break;
	}
	// P -- not shortable, sort of important status info
}
#endif

