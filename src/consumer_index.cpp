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


#include <armadillo>
#include <h5cpp/all>

namespace ch = std::chrono;
/**
creates index for trading days, time intervals for regular time series and the symbol lists
all other operations are depending on the hash tables crated from these indices
*/
void generate_index( const std::string input, const std::string output,
		  const std::string tradingdays_path, const std::string assets_path,
		  const std::string day_begin, const std::string day_end,  unsigned long interval ){

	auto fd = h5::open(output, H5F_ACC_RDWR);
	std::vector<std::string> days = h5::ls( fd, "stats" );
	h5::write(fd,tradingdays_path,days);
}



