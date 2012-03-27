#include <mu/io_test/full.h>

#include <mu/io_test/parser_result.h>
#include <mu/core/errors/error_list.h>
#include <mu/io/parser/parser.h>
#include <mu/io/lexer/lexer.h>
#include <mu/io/source.h>
#include <mu/io/debugging/error.h>
#include <mu/io/debugging/stream.h>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

void mu::io_test::full::run ()
{
	run_1 ();
	run_2 ();
	run_3 ();
}

void mu::io_test::full::run_1 ()
{
	mu::io_test::parser_result result;
	auto stream (boost::make_shared <mu::io::debugging::stream> ());
	auto errors (boost::shared_ptr <mu::core::errors::error_list> (new mu::core::errors::error_list));
	mu::io::parser::parser parser (errors, boost::bind (&mu::io_test::parser_result::operator(), &result, _1));
	mu::io::lexer::lexer lexer (errors, boost::bind (&mu::io::parser::parser::operator (), &parser, _1, _2), stream);
	lexer (L"[thing; thing2;]");
	lexer ();
	assert (result.results.empty ());
	assert (!errors->errors.empty ());
	auto e1 (boost::dynamic_pointer_cast <mu::io::debugging::error> (errors->errors [0]));
	assert (e1.get () != nullptr);
	assert (e1->context == mu::io::debugging::context (stream, 1, 16, 15, 1, 16, 15));
}

void mu::io_test::full::run_2 ()
{
	mu::io_test::parser_result result;
	auto stream (boost::make_shared <mu::io::debugging::stream> ());
	auto errors (boost::shared_ptr <mu::core::errors::error_list> (new mu::core::errors::error_list));
	mu::io::parser::parser parser (errors, boost::bind (&mu::io_test::parser_result::operator(), &result, _1));
	mu::io::lexer::lexer lexer (errors, boost::bind (&mu::io::parser::parser::operator (), &parser, _1, _2), stream);
	lexer (L"[thing; thing2; [");
	lexer ();
	assert (result.results.empty ());
	assert (!errors->errors.empty ());
	auto e1 (boost::dynamic_pointer_cast <mu::io::debugging::error> (errors->errors [0]));
	assert (e1.get () != nullptr);
	assert (e1->context == mu::io::debugging::context (stream, 1, 17, 16, 1, 17, 16));
}

void mu::io_test::full::run_3 ()
{
	mu::io_test::parser_result result;
	auto stream (boost::make_shared <mu::io::debugging::stream> ());
	auto errors (boost::shared_ptr <mu::core::errors::error_list> (new mu::core::errors::error_list));
	mu::io::parser::parser parser (errors, boost::bind (&mu::io_test::parser_result::operator(), &result, _1));
	mu::io::lexer::lexer lexer (errors, boost::bind (&mu::io::parser::parser::operator (), &parser, _1, _2), stream);
	lexer (L"[thing; thing2; thing3 thing4");
	lexer ();
	assert (result.results.empty ());
	assert (!errors->errors.empty ());
	auto e1 (boost::dynamic_pointer_cast <mu::io::debugging::error> (errors->errors [0]));
	assert (e1.get () != nullptr);
	assert (e1->context == mu::io::debugging::context (stream, 1, 24, 23, 1, 29, 28));
}