#include "simple_parser_test_1.h"

#include <lambda_p/core/routine.h>
#include <lambda_p/serialization/simple_parser.h>
#include <lambda_p/tokens/token.h>
#include <lambda_p/serialization/simple.h>

#include <sstream>

lambda_p_test::simple_parser_test_1::simple_parser_test_1(void)
{
}

lambda_p_test::simple_parser_test_1::~simple_parser_test_1(void)
{
}

struct token_vector
{
	void operator () (::lambda_p::tokens::token * token)
	{
		tokens.push_back (token);
	}
	::std::vector < ::lambda_p::tokens::token *> tokens;
};

void lambda_p_test::simple_parser_test_1::run ()
{
	run_1 ();
}

void lambda_p_test::simple_parser_test_1::run_1 ()
{
	::std::wstringstream stream;
	::lambda_p::core::routine routine (0);
	::lambda_p::serialization::simple < ::std::wstringstream> serializer (stream);
	serializer.routine (&routine);
	::std::wstring str (stream.str ());
	stream.seekg (0);
	token_vector tokens;
	::lambda_p::serialization::simple_parser < ::std::wstringstream, token_vector> parser (stream, tokens);
	parser.parse ();
}