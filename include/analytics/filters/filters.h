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
 *   Copyright © <2018-2021> Varga Consulting, Toronto, On     info@vargaconsulting.ca
 *   _________________________________________________________________________________
 */
#ifndef ANALYTICS_ABSTRACT_FILTER_HPP
#define ANALYTICS_ABSTRACT_FILTER_HPP
#include <armadillo>
#include "../generics/all"
#include <cstdint>
//#include <mkl_cblas.h>

// 1. create filter and include filter.hpp
// 2. forward declare in generics/templates.hpp
// 3. update generics/*_impl.hpp
// 4. use filter at your leisure 
	// CRTP
template <typename T, typename Clock>
struct Filter {
	using time_point = typename Clock::time_point;
	using duration = typename Clock::duration;

	void operator()(time_point time, uint64_t stock, float value, uint64_t size);
	void predict( arma::fvec &price );
	void update();
	void resize(size_t N );
	size_t size();
	void zeros();
	void nans();
	/* we are tracking both price and volume */
	arma::fvec price, volume;
	arma::fmat cov;
	std::vector<time_point> start, stop;
	size_t N;
private:
		Filter(){}; // base can't be instantiated
	friend T;
};

template <typename T, typename Clock> void Filter<T, Clock>::operator()(
	time_point time, uint64_t stock, float value, uint64_t size ) {
	static_cast<T*>(this)->update_impl(time, stock, value, size);
}

template <typename T, typename Clock> void Filter<T, Clock>::predict( arma::fvec &price ) {
	static_cast<T*>(this)->predict_impl();
	// use BLAS for better performance
	//cblas_scopy(N, this->price.memptr(), 1, price.memptr(), 1);
	static_cast<T*>(this)->update();
}

template <typename T, typename Clock> void Filter<T, Clock>::update() {
	static_cast<T*>(this)->update_impl();
}

/** resize event is emitted before the trading day is started
 * it's purpose is to provide opportunity to adjust data structure sizes 
 */
template <typename T, typename Clock> void Filter<T, Clock>::resize(size_t N) {
	this->N = N;
	analytics::resize(N,  price, volume, start, stop);
	static_cast<T*>(this)->resize_impl(N);
}
template <typename T, typename Clock> size_t Filter<T, Clock>::size() {
	return N;
}
/* an::zeros calls this back to reset values
*/
template <typename T, typename Clock> void Filter<T, Clock>::zeros() {
	analytics::zeros(price, volume);
	for( auto& t : start) t = time_point::min();
	for( auto& t : stop)  t = time_point::max();
	static_cast<T*>(this)->zeros_impl();
}
template <typename T, typename Clock> void Filter<T, Clock>::nans() {
	static_cast<T*>(this)->nans_impl();
}
#endif
