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

template <typename T, typename Clock> void ones(Filter<T, Clock>& filter){
	filter.ones();
}

// C++ STD TEMPLATES
template <typename T> void ones(std::vector<T>& vec){
	for(int i=0; i<vec.size(); i++) vec[i] = static_cast<T>(1);
}
//ARMA
template <typename T> void ones(arma::Col<T>& vec){
	vec.ones();
}
template <typename T> void ones(arma::Row<T>& vec){
	vec.ones();
}
template <typename T> void ones(arma::Mat<T>& mat){
	mat.ones();
}
template <typename T> void ones(arma::Cube<T>& cube){
	cube.ones();
}


template <typename T, typename Clock, typename... A> void ones(Filter<T, Clock>& head, A&&... tail){
	ones(head); ones(tail...);
}


template <typename T, typename... A> void ones(std::vector<T>& head, A&&... tail){
	ones(head); ones(tail...);
}
template <typename T, typename... A> void ones(arma::Col<T>& head, A&&... tail){
	ones(head); ones(tail...);
}
template <typename T, typename... A> void ones(arma::Row<T>& head, A&&... tail){
	ones(head); ones(tail...);
}
template <typename T, typename...A> void ones(arma::Mat<T>&head, A&&... tail){
	ones(head); ones(tail...);
}
template <typename T, typename...A> void ones(arma::Cube<T>&head, A&&... tail){
	ones(head); ones(tail...);
}


