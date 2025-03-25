
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

template <> inline std::string as( std::string td ){
	return td;
}

//DURATION:
template <> inline std::string as( duration td ){
	return boost::posix_time::to_simple_string( td );
}

template <> inline duration as( std::string du ){
	return boost::posix_time::duration_from_string( du );
}

template <> inline duration as( char const* du ){
	std::string str( du );
	return boost::posix_time::duration_from_string( str );
}

template <> inline duration as( double du ){
	return boost::posix_time::seconds( du );
}

template <> inline duration as( int du ){
	return boost::posix_time::seconds( du );
}

template <> inline duration as( unsigned int du ){
	return boost::posix_time::seconds( du );
}


template <> inline duration as( duration du ){
	return du;
}



//DATE:
template <> inline date as( std::string d ){
	return boost::gregorian::from_simple_string( d );
}

template <> inline date as( char const* d ){
	std::string str( d );
	return boost::gregorian::from_simple_string( d );
}

template <> inline std::string as( date d ){
	return boost::gregorian::to_iso_extended_string( d );
}

template <> inline date as( ptime pt ){
	return pt.date();
}

// POSIX TIME POINT
template <> inline std::string as( ptime pt ){
	return boost::posix_time::to_simple_string( pt );
}
template <> inline ptime as( std::string pt ){
	return boost::posix_time::time_from_string( pt );
}
template <> inline ptime as( char const* pt ){
	std::string pt_( pt );
	return boost::posix_time::time_from_string( pt_ );
}

template <> inline ptime as( time_t pt ){
	return boost::posix_time::from_time_t( pt );
}

template <> inline ptime as( unsigned short pt ){
	return boost::posix_time::from_time_t( static_cast<time_t>( pt ) );
}


template <> inline ptime as( long long pt ){
	return boost::posix_time::from_time_t( static_cast<time_t>( pt ) );
}

template <> inline ptime as( double pt ){
	if( std::isnan( pt ) )
		return boost::posix_time::not_a_date_time;
	else // borrowed: https://stackoverflow.com/questions/1167518/unix-timestamp-to-boostposix-timeptime
		boost::posix_time::ptime b(boost::gregorian::date(1970,1,1),
			boost::posix_time::time_duration(0,0,0, boost::posix_time::time_duration::ticks_per_second() * pt));
}


// DOUBLE
template <> inline double as( ptime pt ){

	auto duration =  pt - as<ptime>( 0ll );
	return duration.total_seconds();
}


template <> inline double as( std::string td ){
	return as<double>( as<ptime>( td ));
}
template <> inline double as( duration du ){
	return static_cast<double>( du.total_seconds() );
}
template <> inline double as( date dt ){
	ptime pt( dt );
	auto duration =  pt - as<ptime>( 0ll );
	return duration.total_seconds();
}


//ITERATOR
template <> inline std::string as( iterator it ){
	return boost::posix_time::to_simple_string( *it );
};

