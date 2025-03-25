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

#ifndef IEX_TOPS156_IMPL_HPP
#define	IEX_TOPS156_IMPL_HPP

template <typename T>
void iex::Transport<T>::tops_v156( const iex::tops::v156::message * msg  ){

	using namespace tops;
	time_point tp = time_point( duration( msg->hdr.time ));
	const v156::quote_update* qu = &msg->qu;
	const v156::trade_report* tr = &msg->tr;
	const v156::trade_break*  tb = &msg->tb;

	switch( msg->hdr.type ){
		case 'Q': // quote update
			if( qu->ask_size ) this->ask(tp, msg->hdr.symbol, 1e-4*qu->ask_price, qu->ask_size, 0);
			if( qu->bid_size ) this->bid(tp, msg->hdr.symbol, 1e-4*qu->bid_price, qu->bid_size, 0);
			break;
		case 'T': // trade report
			this->trade_report(tp, msg->hdr.symbol, 1e-4*tr->price, tr->size, msg->hdr.flag );
			break;
		case 'B': // trade break
			this->trade_break(tp, msg->hdr.symbol, tb->price, tb->size, msg->hdr.flag );
			break;
	}
}
#endif

