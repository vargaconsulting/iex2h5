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
#ifndef ANALYTICS_VOLUME_COMPENSATED_HPP
#define ANALYTICS_VOLUME_COMPENSATED_HPP

#include "filters.h"

#define SIGNAL_DELAY_FILTER_EMA 3
struct VolFilter : public Filter<VolFilter> {

	VolFilter():factor(2.0 / (SIGNAL_DELAY_FILTER_EMA+1.0)){}
	// ultra high frequency update 
	void update_impl(uint64_t  stock, float price, uint64_t volume ){
		vol[stock] += volume;
		pr[stock]  += volume * price;
	}
	void predict_impl(){}
	// low res timer
	void update_impl(){
		for( uint64_t stock=0; stock<N; stock++ ){
			pr[stock] = vol[stock] > 0 ? pr[stock] / vol[stock] : price[stock];
			// take the EMA of volume compensated price, then update 
			if( price[stock] > 0 )
				price[stock] = factor * pr[stock] + (1.0 - factor) * price[stock];
			 else
				price[stock] = pr[stock];
		}
		analytics::zeros(pr,vol);
	}

	void resize_impl(size_t N){
		namespace an = analytics;
		an::resize(N, pr,vol);
		an::zeros(pr,vol);
	}
	void zeros_impl(){
		analytics::zeros(pr,vol);
	}
	void nans_impl(){
		analytics::nans(pr,vol);
	}

	float factor;
	arma::fvec pr,vol;
};
#undef SIGNAL_DELAY_FILTER_EMA
#endif
