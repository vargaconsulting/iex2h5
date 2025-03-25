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

template <int P, typename T> void round( T&& v ){
	for(int i=0; i<v.n_elem; i++ )	v[i] = std::ceil( pow(10,P) * v[i] ) /  pow(10,P);
}

template <int P, typename T, typename... A> void round(T&& first, A&&... args ){
	round<P>(first); 
	round<P>(args...);
}

template <typename T> void ceil( T&& v ){
	for(int i=0; i<v.n_elem; i++ )	v[i] = std::ceil( v[i] );
}

template <typename T, typename... A> void ceil(T&& first, A&&... args ){
	round<>(first);
	round<>(args...);
}

template <typename T> void floor( T&& v ){
	for(int i=0; i<v.n_elem; i++ )	v[i] = std::floor( v[i] );
}

template <typename T, typename... A> void floor(T&& first, A&&... args ){
	round<>(first);
	round<>(args...);
}





