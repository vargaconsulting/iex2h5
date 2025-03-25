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

	//STD::VECTOR
	template <typename T, typename Clock> void resize(size_t N, Filter<T, Clock>& filter){
		filter.resize(N);
	}
	template <typename T, typename Clock> void resize(size_t N, EMA<T, Clock>& filter){
		filter.resize(N);
	}


	template <typename T> void resize(size_t N, std::vector<T>& vec){
		vec.resize(N);
	}
	//ARMA
	template <typename T> void resize(size_t N, arma::Col<T>& vec){
		vec.set_size(N);
	}
	template <typename T> void resize(size_t N, arma::Row<T>& vec){
		vec.set_size(N);
	}
	template <typename T> void resize(size_t R, size_t C, arma::Mat<T>& mat){
		mat.set_size(R,C);
	}
	template <typename T> void resize(size_t R, size_t C, arma::SpMat<T>& mat){
		mat.set_size(R,C);
	}
	template <typename T> void resize(size_t R, size_t C, size_t S, arma::Cube<T>& cube){
		cube.set_size(R,C,S);
	}


	// peel variables off from stack
	template <typename T, typename Clock, typename... A> void resize(size_t N, Filter<T, Clock>& head, A&&... tail){
		resize(N, head); resize(N, tail...);
	}
	template <typename T, typename Clock, typename... A> void resize(size_t N, EMA<T, Clock>& head, A&&... tail){
		resize(N, head); resize(N, tail...);
	}
	template <typename T, typename... A> void resize(size_t N, std::vector<T>& head, A&&... tail){
		resize(N, head); resize(N, tail...);
	}
	template <typename T, typename... A> void resize(size_t N, arma::Col<T>& head, A&&... tail){
		resize(N, head); resize(N, tail...);
	}
	template <typename T, typename... A> void resize(size_t N, arma::Row<T>& head, A&&... tail){
		resize(N, head); resize(N, tail...);
	}
	template <typename T, typename...A> void resize(size_t R, size_t C, arma::Mat<T>&head, A&&... tail){
		resize(R,C,head); resize(R,C, tail...);
	}
	template <typename T, typename...A> void resize(size_t R, size_t C, arma::SpMat<T>&head, A&&... tail){
		resize(R,C,head); resize(R,C, tail...);
	}
	template <typename T, typename...A> void resize(size_t R, size_t C, size_t S, arma::Cube<T>&head, A&&... tail){
		resize(R,C,S, head); resize(R,C,S, tail...);
	}
// END
}


// FILL
namespace analytics {
	template <typename V, typename T> void fill(V value, T&& x ){
		x.fill(value);
	}
	template <typename V, typename T, typename... A> void fill(V value,  T&& head, A&&... tail ){
		fill(value, head); fill(value, tail...);
	}

	template <typename T, typename... A> void NaN(T&& head, A&&... tail){
		fill(arma::datum::nan, head); fill(arma::datum::nan, tail...);
	}
	template <typename T, typename... A> void zeros(T&& first, A&&... args){
		fill(0, first); fill(0, args...);
	}
	template <typename T, typename... A> void ones(T&& first, A&&... args){
		fill(1, first); fill(1, args...);
	}
}





