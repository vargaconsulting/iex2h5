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

template <typename T, typename F> std::vector<T> as( const std::vector<F> &from ){}
template <typename T, typename F> std::vector<T> as( const arma::Col<F> &from ){}
template <typename T, typename F> std::vector<T> as( const arma::Row<F> &from ){}


template<typename duration  > std::vector<std::string> as( const std::vector<duration>& from, std::string fmt="%H:%M:S" ){
	std::vector<std::string> to( from.size() );
		for(int i=0; i<from.size(); i++ ) to[i] = date::format(fmt, from[i]);
	return to;
}



