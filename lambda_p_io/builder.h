#pragma once

#include <lambda_p_io/source.h>
#include <lambda_p_io/lexer/lexer.h>
#include <lambda_p_io/parser/parser.h>
#include <lambda_p_io/analyzer/analyzer.h>
#include <lambda_p/errors/error_list.h>

#include <vector>

namespace lambda_p_io
{
	class builder
	{
	public:
		builder ();
		std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extension>> extensions ();
		boost::shared_ptr <lambda_p::errors::error_list> errors;
		lambda_p_io::analyzer::analyzer analyzer;
		lambda_p_io::parser::parser parser;
		lambda_p_io::lexer::lexer lexer;
		void operator () (boost::shared_ptr <lambda_p::routine>);
		std::vector <boost::shared_ptr <lambda_p::routine>> routines;
	};
}
