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
// C++ STD TEMPLATES
template <typename T> void zeros2nans( std::vector<T>& vec){
	for(int i=0; i<vec.size(); i++ ) 	if( vec[i] == 0  ) vec[i] = arma::datum::nan;
}
//ARMA
template <typename T> void zeros2nans( arma::Col<T>& vec){
	for(int i=0; i<vec.n_elem; i++ ) 	if( vec[i] == 0  ) vec[i] = arma::datum::nan;
}
template <typename T> void zeros2nans( arma::Row<T>& vec){
	for(int i=0; i<vec.n_elem; i++ ) 	if( vec[i] == 0  ) vec[i] = arma::datum::nan;
}
template <typename T> void zeros2nans( arma::Mat<T>& mat){
	for(int i=0; i<mat.n_elem; i++ ) 	if( mat[i] == 0  ) mat[i] = arma::datum::nan;
}
template <typename T> void zeros2nans( arma::Cube<T>& cube){
	for(int i=0; i<cube.n_elem; i++ ) 	if( cube[i] == 0  ) cube[i] = arma::datum::nan;
}


// RECURSIVE TEMPLATES
template <typename T, typename Clock, typename... A> void zeros2nans( Filter<T, Clock>& head, A&&... tail){
	zeros2nans(head); zeros2nans(tail...);
}

template <typename T, typename... A> void zeros2nans(std::vector<T>& head, A&&... tail){
	zeros2nans(head); zeros2nans(tail...);
}
template <typename T, typename... A> void zeros2nans(arma::Col<T>& head, A&&... tail){
	zeros2nans(head); zeros2nans(tail...);
}
template <typename T, typename... A> void zeros2nans(arma::Row<T>& head, A&&... tail){
	zeros2nans(head); zeros2nans(tail...);
}
template <typename T, typename... A> void zeros2nans(arma::Mat<T>&head, A&&... tail){
	zeros2nans(head); zeros2nans(tail...);
}
template <typename T, typename... A> void zeros2nans(arma::Cube<T>&head, A&&... tail){
	zeros2nans(head); zeros2nans(tail...);
}


// CUSTOM TEMPLATES
// C++ STD TEMPLATES
template <typename T> void nans2zeros( std::vector<T>& vec){
	for(int i=0; i<vec.size(); i++ ) 	if( std::isnan( vec[i] ) ) vec[i] = 0.;
}
//ARMA
template <typename T> void nans2zeros( arma::Col<T>& vec){
	for(int i=0; i<vec.n_elem; i++ ) 	if( std::isnan(vec[i]) ) vec[i] = 0.;
}
template <typename T> void nans2zeros( arma::Row<T>& vec){
	for(int i=0; i<vec.n_elem; i++ ) 	if( std::isnan( vec[i])  ) vec[i] = 0.;
}
template <typename T> void nans2zeros( arma::Mat<T>& mat){
	for(int i=0; i<mat.n_elem; i++ ) 	if( std::isnan( mat[i] ) ) mat[i] = 0.;
}
template <typename T> void nans2zeros( arma::Cube<T>& cube){
	for(int i=0; i<cube.n_elem; i++ ) 	if( std::isnan(cube[i])  ) cube[i] = 0.;
}


// RECURSIVE TEMPLATES
template <typename T, typename Clock, typename... A> void nans2zeros( Filter<T, Clock>& head, A&&... tail){
	nans2zeros(head); nans2zeros(tail...);
}
template <typename T, typename... A> void nans2zeros(std::vector<T>& head, A&&... tail){
	zeros2nans(head); nans2zeros(tail...);
}
template <typename T, typename... A> void nans2zeros(arma::Col<T>& head, A&&... tail){
	nans2zeros(head); nans2zeros(tail...);
}
template <typename T, typename... A> void nans2zeros(arma::Row<T>& head, A&&... tail){
	nans2zeros(head); nans2zeros(tail...);
}
template <typename T, typename... A> void nans2zeros(arma::Mat<T>&head, A&&... tail){
	nans2zeros(head); nans2zeros(tail...);
}
template <typename T, typename... A> void nans2zeros(arma::Cube<T>&head, A&&... tail){
	nans2zeros(head); nans2zeros(tail...);
}




