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

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/variant.hpp>

#include <iostream>
#include <string>

namespace grammar {
	using namespace boost::spirit::x3;
	struct values_ : symbols<unsigned>{} values;
	using namespace std;

	auto RESET 	= [&](auto& ctx){ _val(ctx).clear(); };
	auto PUSH 	= [&](auto& ctx){  _val(ctx).push_back(_attr(ctx)); };
	auto APPEND = [&](auto& ctx){
		for(auto i:_attr(ctx))
					_val(ctx).push_back(i);
	};
	auto RANGE 	= [&](auto& ctx){
		auto in = _attr(ctx); auto& out = _val(ctx);
		for( auto i = front(in); i<= back(in); i++) 
										out.push_back(i);
	};
	auto NONE = [&](auto& ctx){ _val(ctx).clear(); };
	auto ALL = [&](auto& ctx){
		values.for_each([&](std::string s, unsigned v ){
				_val(ctx).push_back(v);
				});
	};
	auto ERROR = [&](auto& ctx){ };
	auto ACCEPT = [&](auto& ctx){
		std::vector<unsigned> &out = _val(ctx);
		std::vector<unsigned> &v = boost::get<std::vector<unsigned>>( _attr(ctx) );
		std::copy(v.begin(),v.end(), std::back_inserter(out) );
		std::sort(out.begin(), out.end());
		auto from = std::unique(out.begin(), out.end() );
		out.erase(from,out.end());
	};

	rule<class symbols, std::vector<unsigned>> const symbols;
	rule<class numbers, std::vector<unsigned>> const numbers;
	rule<class range,   std::vector<unsigned>> const range;
	rule<class ranges,  std::vector<unsigned>> const ranges;
	rule<class literal, std::vector<unsigned>> const literal;
	rule<class line,    std::vector<unsigned>> const line;

	auto const symbols_def = no_case[values[PUSH] % ','];
	auto const range_def   = (uint_ >> ':' >> uint_)[RANGE];
	auto const ranges_def  = range[APPEND] % ',';
	auto const numbers_def = uint_[PUSH] % ',';
	auto const literal_def = lit(":all")[ALL] | lit(":none")[NONE];

	auto const line_def    =  (
			(symbols|ranges|numbers|literal) >> eoi) [ACCEPT]
		;

	BOOST_SPIRIT_DEFINE(symbols,numbers,range,ranges,literal,line);
	std::vector<unsigned> parse( const std::vector<std::string>& symbols, const std::string& input   ){
		std::vector<unsigned> result;
		std::string::const_iterator iter = input.begin();
		std::string::const_iterator end = input.end();
		values.clear();
		for( int i=0;i<symbols.size(); i++)
			values.add(symbols[i],i);
		if( parse(iter, end, line, result) )
			return result;
		else
			std::cout << "Parsing failed:" << std::string(iter,end) << endl;
		return result;
	}
}


