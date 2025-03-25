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

#ifndef IEX_TRANSPORT125_IMPL_HPP
#define	IEX_TRANSPORT125_IMPL_HPP


template <typename T>
iex::Transport<T>::Transport() : count(0), is_market_opened(false), is_market_closed(false), is_first_beat(true) {
}

template <typename T>
void iex::Transport<T>::transport_handler( const iex::transport::header* segment  ){
	using namespace std;
	using namespace date;

	if( !count ) today = date::floor<date::days>( time_point(duration( segment->time) ) );
	auto now = time_point(duration( segment->time) );

	// trigger opening market event
	if( now > today + this->start && !is_market_opened )
		is_market_opened = true,this->day_begin( now );

	char* cursor = (char*)(segment + 1); // the first message
	if( is_market_opened && !is_market_closed)
		// a segment may contain multiple messages, we are to iterate through them
		for( int i=0; i < segment->message_count; i++ ){
			// make sure to trigger this timer event before processing the current
			// HFT event, so the current state of client will not contain the event that tripped
			// timer
			if( now - last_time >= this->heart_beat_interval ){
					last_time = date::floor<std::chrono::seconds>( now );

					if( !is_first_beat ) 
						this->heart_beat( last_time );
					else
						is_first_beat = false;
			}
			const Block* block =  (Block*) cursor;
			switch( segment[i].protocol_id ) {
				case IEX_DEEPS_v105: deeps_v105( (iex::deeps::v105::message*) &block->hdr ); break;
				case IEX_TOPS_v156: tops_v156( (iex::tops::v156::message*)  &block->hdr ); break;
				case IEX_TOPS_v163: tops_v163( (iex::tops::v163::message*)  &block->hdr ); break;
				default: ;
			}
			cursor += (block->length+sizeof(Block::length)); // move cursor to next block,
		}
	//closing market
	if( now > today + this->stop && is_market_opened && !is_market_closed )
		is_market_closed = true, this->day_end( now );
	count++;
}
#endif

