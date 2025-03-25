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
#ifndef ANALYTICS_EMA_FILTER_HPP
#define ANALYTICS_EMA_FILTER_HPP

#define SIGNAL_DELAY_FILTER_EMA 2

#include "filters.h"
template <typename Clock>
struct EMAFilter : public Filter<EMAFilter<Clock>, Clock> {
	using time_point = typename Clock::time_point;

//	EMAFilter():factor(2.0 / (SIGNAL_DELAY_FILTER_EMA+1.0)){}
	EMAFilter():factor(.05){}
	// ultra high frequency update 
	void update_impl(time_point time, uint64_t  stock, float price, uint64_t volume){
		//std::cout << std::dec << stock <<" "<<price << " " << volume << "\n";
		if( this->price[stock] > 0 )
			this->price[stock] = factor * price + (1.0 - factor) * this->price[stock];
		else
			this->price[stock] = price;
	}
	// low res timer
	void predict_impl(){
	}
	// low res timer
	void update_impl(){
	}

	void zeros_impl(){
	}
	void nans_impl(){
	}

	void resize_impl(size_t N){
	}

	float factor;
};
#undef SIGNAL_DELAY_FILTER_EMA
#endif
