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

#ifndef IO_PRODUCER_IMPL_HPP
#define IO_PRODUCER_IMPL_HPP


template <typename Derived, typename Consumer>
void io::Producer<Derived,Consumer>::run( Consumer& ref, duration start_, duration stop_ ){
	this->consumer = &ref; start = start_; stop = stop_;
	static_cast<Derived*>(this)->run_impl();
}

template <typename D, typename C>
void io::Producer<D,C>::trade_report(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){
	consumer->trade_report(time,stock,price,size,flag);
}

template <typename D, typename C>
void io::Producer<D,C>::ask(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){
	consumer->ask(time,stock,price,size,flag);
}

template <typename D, typename C>
void io::Producer<D,C>::bid(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){
	consumer->bid(time,stock,price,size,flag);
}

template <typename D, typename C>
void io::Producer<D,C>::trade_break(time_point time,  uint64_t stock, float price, uint64_t size, uint8_t flag ){
	consumer->trade_break(time,stock,price,size,flag);
}

template <typename D, typename C>
void io::Producer<D,C>::heart_beat( time_point tp ){
	consumer->heart_beat( tp );
}

template <typename D, typename C>
void io::Producer<D,C>::begin( time_point tp  ){
	consumer->begin(tp);
}

template <typename D, typename C>
void io::Producer<D,C>::end( time_point tp  ){
	consumer->end(tp);
}

template <typename D, typename C> 
void io::Producer<D,C>::day_end( time_point day ){
	consumer->day_end( day );
}

template <typename D, typename C> 
void io::Producer<D,C>::day_begin( time_point day ){
	consumer->day_begin( day );
}
#endif
