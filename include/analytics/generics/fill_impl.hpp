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

// CUSTOM TEMPLATES
template <typename T, typename Clock> void fill(T value, Filter<T, Clock>& filter){
	filter.fill(value);
}

// C++ STD TEMPLATES
template <typename T> void fill(T value, std::vector<T>& vec){
	for(int i=0; i<vec.size(); i++) vec[i] = value;
}
//ARMA
template <typename T> void fill(T value, arma::Col<T>& vec){
	vec.fill(value);
}
template <typename T> void fill(T value, arma::Row<T>& vec){
	vec.fill(value);
}
template <typename T> void fill(T value, arma::Mat<T>& mat){
	mat.fill(value);
}
template <typename T> void fill(T value, arma::SpMat<T>& mat){
	mat.fill(value);
}
template <typename T> void fill(T value, arma::Cube<T>& cube){
	cube.fill(value);
}


// RECURSIVE TEMPLATES
template <typename T, typename Clock, typename... A> void fill(T value, Filter<T, Clock>& head, A&&... tail){
	fill(value, head); fill(value, tail...);
}


template <typename T, typename... A> void fill(T value, std::vector<T>& head, A&&... tail){
	fill(value, head); fill(value, tail...);
}
template <typename T, typename... A> void fill(T value, arma::Col<T>& head, A&&... tail){
	fill(value, head); fill(value, tail...);
}
template <typename T, typename... A> void fill(T value, arma::Row<T>& head, A&&... tail){
	fill(value, head); fill(value, tail...);
}
template <typename T, typename...A> void fill(T value, arma::Mat<T>&head, A&&... tail){
	fill(value, head); fill(value, tail...);
}
template <typename T, typename...A> void fill(T value, arma::SpMat<T>&head, A&&... tail){
	fill(value, head); fill(value, tail...);
}
template <typename T, typename...A> void fill(T value, arma::Cube<T>&head, A&&... tail){
	fill(value, head); fill(value, tail...);
}

