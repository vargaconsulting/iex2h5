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

#include <armadillo>

#ifndef AN_GENERICS_H
#define	AN_GENERICS_H

// implements: resize, clear, fill, zeros, ones such that:
//     op( [args], objects... ); an::fill(1, vec1, mat2, cube3, std_vec4, ...); 

namespace analytics {
	template <typename T> void zeros( T& v ){ v = T(0); };

	template <typename T, typename Clock> class Filter;
	template <typename T, typename Clock> class EMA;
	//FORWARD DECL
	template <typename T, typename Clock, typename...A> void fill(T, Filter<T, Clock>&, A&&...);
	template <typename T, typename Clock, typename...A> void fill(T, EMA<T, Clock>&, A&&...);
	template <typename T, typename...A> void fill(T, std::vector<T>&, A&&...);
	template <typename T, typename...A> void fill(T, arma::Row<T>&, A&&...);
	template <typename T, typename...A> void fill(T, arma::Col<T>&, A&&...);
	template <typename T, typename...A> void fill(T, arma::Mat<T>&, A&&...);
	template <typename T, typename...A> void fill(T, arma::SpMat<T>&, A&&...);
	template <typename T, typename...A> void fill(T, arma::Cube<T>&, A&&...);

	template <typename T, typename...A> void zeros( unsigned long int&, A&&...);
	template <typename T, typename...A> void zeros( int&, A&&...);
	template <typename T, typename Clock, typename...A> void zeros( Filter<T, Clock>&, A&&...);
	template <typename T, typename Clock, typename...A> void zeros( EMA<T, Clock>&, A&&...);
	template <typename T, typename...A> void zeros( std::vector<T>&, A&&...);
	template <typename T, typename...A> void zeros( arma::Row<T>&, A&&...);
	template <typename T, typename...A> void zeros( arma::Col<T>&, A&&...);
	template <typename T, typename...A> void zeros( arma::Mat<T>&, A&&...);
	template <typename T, typename...A> void zeros( arma::SpMat<T>&, A&&...);
	template <typename T, typename...A> void zeros( arma::Cube<T>&, A&&...);

	template <typename T, typename Clock, typename...A> void ones( Filter<T, Clock>&, A&&...);
	template <typename T, typename Clock, typename...A> void ones( EMA<T, Clock>&, A&&...);
	template <typename T, typename...A> void ones( std::vector<T>&, A&&...);
	template <typename T, typename...A> void ones( arma::Row<T>&, A&&...);
	template <typename T, typename...A> void ones( arma::Col<T>&, A&&...);
	template <typename T, typename...A> void ones( arma::Mat<T>&, A&&...);
	template <typename T, typename...A> void ones( arma::SpMat<T>&, A&&...);
	template <typename T, typename...A> void ones( arma::Cube<T>&, A&&...);

	//TODO: only allow floating point T types
	template <typename T, typename...A> void nans( std::vector<T>&, A&&...);
	template <typename T, typename...A> void nans( arma::Row<T>&, A&&...);
	template <typename T, typename...A> void nans( arma::Col<T>&, A&&...);
	template <typename T, typename...A> void nans( arma::Mat<T>&, A&&...);
	template <typename T, typename...A> void nans( arma::SpMat<T>&, A&&...);
	template <typename T, typename...A> void nans( arma::Cube<T>&, A&&...);


	template <typename T, typename Clock, typename...A> void resize(size_t, Filter<T, Clock>&, A&&...);
	template <typename T, typename Clock, typename...A> void resize(size_t, EMA<T, Clock>&, A&&...);
	template <typename T, typename...A> void resize(size_t, std::vector<T>&, A&&...);
	template <typename T, typename...A> void resize(size_t, arma::Row<T>&, A&&...);
	template <typename T, typename...A> void resize(size_t, arma::Col<T>&, A&&...);
	template <typename T, typename...A> void resize(size_t, size_t, arma::Mat<T>&, A&&...);
	template <typename T, typename...A> void resize(size_t, size_t, arma::SpMat<T>&, A&&...);
	template <typename T, typename...A> void resize(size_t, size_t, size_t, arma::Cube<T>&, A&&...);

	//MISC
	template <int P, typename T, typename... A> void round(T&& first, A&&... args );
	template <typename T, typename... A> void ceil(T&& first, A&&... args );
	template <typename T, typename... A> void floor(T&& first, A&&... args );

	template <typename T, typename...A> void zeros2nans( std::vector<T>&, A&&...);
	template <typename T, typename...A> void zeros2nans( arma::Row<T>&, A&&...);
	template <typename T, typename...A> void zeros2nans( arma::Col<T>&, A&&...);
	template <typename T, typename...A> void zeros2nans( arma::Mat<T>&, A&&...);
	template <typename T, typename...A> void zeros2nans( arma::Cube<T>&, A&&...);


	#include "resize_impl.hpp"
	#include "fill_impl.hpp"
	#include "ones_impl.hpp"
	#include "nans_impl.hpp"
	#include "zeros_impl.hpp"
	#include "zeros2nans_impl.hpp"
	#include "misc_impl.hpp"
}

#endif	

