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

template <typename T, typename Clock> void zeros(Filter<T, Clock>& filter){
	filter.zeros();
}
// C++ STD TEMPLATES
template <typename T> void zeros(std::vector<T>& vec){
	for(int i=0; i<vec.size(); i++) vec[i] = static_cast<T>(0);
}
//ARMA
template <typename T> void zeros(arma::Col<T>& vec){
	vec.zeros();
}
template <typename T> void zeros(arma::Row<T>& vec){
	vec.zeros();
}
template <typename T> void zeros(arma::Mat<T>& mat){
	mat.zeros();
}
template <typename T> void zeros(arma::Cube<T>& cube){
	cube.zeros();
}

template <typename T, typename Clock, typename... A> void zeros(Filter<T, Clock>& head, A&&... tail){
	zeros(head); zeros(tail...);
}
template <typename T, typename... A> void zeros(std::vector<T>& head, A&&... tail){
	zeros(head); zeros(tail...);
}
template <typename T, typename... A> void zeros(arma::Col<T>& head, A&&... tail){
	zeros(head); zeros(tail...);
}
template <typename T, typename... A> void zeros(arma::Row<T>& head, A&&... tail){
	zeros(head); zeros(tail...);
}
template <typename T, typename...A> void zeros(arma::Mat<T>&head, A&&... tail){
	zeros(head); zeros(tail...);
}
template <typename T, typename...A> void zeros(arma::Cube<T>&head, A&&... tail){
	zeros(head); zeros(tail...);
}

