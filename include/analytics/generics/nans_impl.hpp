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

template <typename T, typename Clock> void nans(Filter<T, Clock>& filter){
	filter.nans();
}

// C++ STD TEMPLATES
template <typename T> void nans(std::vector<T>& vec){
	for(int i=0; i<vec.size(); i++) vec[i] = std::numeric_limits<T>::quiet_NaN();
}
//ARMA
template <typename T> void nans(arma::Col<T>& vec){
	vec.fill(arma::datum::nan);
}
template <typename T> void nans(arma::Row<T>& vec){
	vec.fill(arma::datum::nan);

}
template <typename T> void nans(arma::Mat<T>& mat){
	mat.fill(arma::datum::nan);
}
template <typename T> void nans(arma::Cube<T>& cube){
	cube.fill(arma::datum::nan);
}


template <typename T, typename Clock, typename... A> void nans(Filter<T, Clock>& head, A&&... tail){
	nans(head); nans(tail...);
}


template <typename T, typename... A> void nans(std::vector<T>& head, A&&... tail){
	nans(head); nans(tail...);
}
template <typename T, typename... A> void nans(arma::Col<T>& head, A&&... tail){
	nans(head); nans(tail...);
}
template <typename T, typename... A> void nans(arma::Row<T>& head, A&&... tail){
	nans(head); nans(tail...);
}
template <typename T, typename...A> void nans(arma::Mat<T>&head, A&&... tail){
	nans(head); nans(tail...);
}
template <typename T, typename...A> void nans(arma::Cube<T>&head, A&&... tail){
	nans(head); nans(tail...);
}


