#include "values.h"

#include <lambda_p_io_test/parser_result.h>
#include <lambda_p/errors/error_list.h>
#include <lambda_p_io/parser/parser.h>
#include <lambda_p_io/lexer/lexer.h>
#include <lambda_p_io/source.h>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

void lambda_p_io_test::values::run ()
{
	run_1 ();
}

void lambda_p_io_test::values::run_1 ()
{
	lambda_p_io_test::parser_result result;
	auto errors (boost::shared_ptr <lambda_p::errors::error_list> (new lambda_p::errors::error_list));
	lambda_p_io::parser::parser parser (errors, boost::bind (&lambda_p_io_test::parser_result::operator(), &result, _1));
	lambda_p_io::lexer::lexer lexer (errors, boost::bind (&lambda_p_io::parser::parser::operator (), &parser, _1, _2));
	lambda_p_io::source source (boost::bind (&lambda_p_io::lexer::lexer::operator (), &lexer, _1));
	source (L"[ thing");
	source ();
	assert (result.results.empty ());
	assert (!errors->errors.empty ());
	auto e1 (errors->errors [0]);
	assert (e1.second == lambda_p::context (1, 1, 0, 1, 8, 7));
}