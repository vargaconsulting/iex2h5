/*_______________________________________________________________________________

   VARGA CONSULTING
  __________________

   [2010] - [2016] Varga Consulting
   All Rights Reserved.
  ______________________________________________________________________________
  NOTICE: All information contained herein is, and remains the property of Varga
  Consulting and its suppliers, if  any. The intellectual and technical concepts
  contained herein are proprietary to Varga Consulting and its suppliers and may
  be covered by U.S. and Foreign  Patents, patents in process, and are protected
  by  trade  secret or  copyright  law.  Dissemination  of this  information  or
  reproduction  of this  material  is strictly  forbidden  unless prior  written
  permission is obtained from Varga Consulting.
  ______________________________________________________________________________

  Copyright (c) <2016> < Copyright © 2016 Steven Varga, Toronto, On>

  Contact: Steven Varga
           <steven@vargaconsulting.ca>
           2016 Toronto, On Canada
  ______________________________________________________________________________*/

#ifndef H5_PTIME_H
#define	H5_PTIME_H

#include "boost/date_time/posix_time/posix_time.hpp"

namespace h5 {
	typedef std::vector<boost::posix_time::time_duration> duration;
	typedef std::vector<boost::posix_time::ptime> ptime;
	typedef std::vector<boost::gregorian::date> date;
	typedef std::vector<boost::posix_time::time_period> period;
	typedef std::vector<boost::posix_time::time_iterator> iterator;
	typedef std::vector<std::string> string;
}

#endif
