#include <mu/io/parser/parser.h>
#include <mu/test_entry/io/parser_result.h>
#include <mu/io/tokens/stream_end.h>
#include <mu/io/tokens/divider.h>
#include <mu/io/tokens/identifier.h>
#include <mu/io/tokens/left_square.h>
#include <mu/io/tokens/right_square.h>
#include <mu/io/ast/expression.h>
#include <mu/io/ast/identifier.h>
#include <mu/core/errors/error_list.h>
#include <mu/io/ast/cluster.h>

#include <boost/bind.hpp>

#include <gtest/gtest.h>

#include <gc_cpp.h>

TEST (io_test, parser1)
{
	mu::io_test::parser_result result;
	auto errors (new (GC) mu::core::errors::error_list);
	mu::io::parser::parser parser (errors, boost::bind (&mu::io_test::parser_result::operator(), &result, _1));
	parser (new mu::io::tokens::stream_end, mu::io::debugging::context (0, 0, 0, 0, 0, 0));
	EXPECT_EQ (result.results.size (), 1);
	EXPECT_EQ (result.results [0]->expressions.size (), 0);
	EXPECT_EQ (errors->errors.empty (), true);
}

TEST (io_test, parser2)
{
	mu::io_test::parser_result result;
	auto errors (new (GC) mu::core::errors::error_list);
	mu::io::parser::parser parser (errors, boost::bind (&mu::io_test::parser_result::operator(), &result, _1));
	parser (new mu::io::tokens::left_square, mu::io::debugging::context (1, 1, 0, 1, 1, 0));
	parser (new mu::io::tokens::right_square, mu::io::debugging::context (1, 2, 1, 1, 2, 1));
	parser (new mu::io::tokens::stream_end, mu::io::debugging::context (1, 3, 2, 1, 3, 2));
	EXPECT_EQ (result.results.size (), 1);
	auto n1 (result.results [0]);
	EXPECT_EQ (n1->expressions.size (), 1);
	auto e1 (n1->expressions [0]);
	EXPECT_NE (e1, nullptr);
	EXPECT_EQ (e1->values.empty (), true);
	EXPECT_EQ (e1->full_name->string.empty (), true);
	EXPECT_EQ (e1->individual_names.empty (), true);
	EXPECT_EQ (e1->context, mu::io::debugging::context (1, 1, 0, 1, 2, 1));	
}

TEST (io_test, parser3)
{
	mu::io_test::parser_result result;
	auto errors (new (GC) mu::core::errors::error_list);
	mu::io::parser::parser parser (errors, boost::bind (&mu::io_test::parser_result::operator(), &result, _1));
	parser (new mu::io::tokens::left_square, mu::io::debugging::context (1, 1, 0, 1, 1, 0));
	parser (new mu::io::tokens::identifier (std::wstring (L"t1")), mu::io::debugging::context (1, 2, 1, 1, 3, 2));
	parser (new mu::io::tokens::right_square, mu::io::debugging::context (1, 4, 3, 1, 4, 3));
	parser (new mu::io::tokens::stream_end, mu::io::debugging::context (1, 5, 4, 1, 5, 4));
	EXPECT_EQ (result.results.size (), 1);
	EXPECT_EQ (result.results [0]->expressions.size (), 1);
	auto e1 (result.results [0]->expressions [0]);
	EXPECT_EQ (e1->context, mu::io::debugging::context (1, 1, 0, 1, 4, 3));
	EXPECT_EQ (e1->values.size (), 1);
	auto i1 (dynamic_cast <mu::io::ast::identifier*> (e1->values [0]));
	EXPECT_EQ (i1->string, std::wstring (L"t1"));
	EXPECT_EQ (i1->context, mu::io::debugging::context (1, 2, 1, 1, 3, 2));
	EXPECT_EQ (e1->full_name->string.empty (), true);
	EXPECT_EQ (e1->individual_names.empty (), true);
}

TEST (io_test, parser4)
{
	mu::io_test::parser_result result;
	auto errors (new (GC) mu::core::errors::error_list);
	mu::io::parser::parser parser (errors, boost::bind (&mu::io_test::parser_result::operator(), &result, _1));
	parser (new mu::io::tokens::left_square, mu::io::debugging::context (1, 1, 0, 1, 1, 0));
	parser (new mu::io::tokens::divider, mu::io::debugging::context (1, 2, 1, 1, 2, 1));
	parser (new mu::io::tokens::identifier (std::wstring (L"t1")), mu::io::debugging::context (1, 3, 2, 1, 4, 3));
	parser (new mu::io::tokens::right_square, mu::io::debugging::context (1, 5, 4, 1, 5, 4));
	parser (new mu::io::tokens::stream_end, mu::io::debugging::context (1, 6, 5, 1, 6, 5));
	EXPECT_EQ (result.results.size (), 1);
	EXPECT_EQ (result.results [0]->expressions.size (), 1);
	auto e1 (result.results [0]->expressions [0]);
	EXPECT_EQ (e1->values.empty (), true);
	EXPECT_EQ (e1->individual_names.size (), 1);
	EXPECT_EQ (e1->context, mu::io::debugging::context (1, 1, 0, 1, 5, 4));
	auto i1 (e1->individual_names [0]);
	EXPECT_EQ (i1->string, std::wstring (L"t1"));
	EXPECT_EQ (e1->full_name->string.empty (), true);
}

TEST (io_test, parser5)
{
	mu::io_test::parser_result result;
	auto errors (new (GC) mu::core::errors::error_list);
	mu::io::parser::parser parser (errors, boost::bind (&mu::io_test::parser_result::operator(), &result, _1));
	parser (new mu::io::tokens::left_square, mu::io::debugging::context (1, 1, 0, 1, 1, 0));
	parser (new mu::io::tokens::divider, mu::io::debugging::context (1, 2, 1, 1, 2, 1));
	parser (new mu::io::tokens::divider, mu::io::debugging::context (1, 3, 2, 1, 3, 2));
	parser (new mu::io::tokens::identifier (std::wstring (L"t1")), mu::io::debugging::context (1, 4, 3, 1, 5, 4));
	parser (new mu::io::tokens::right_square, mu::io::debugging::context (1, 6, 5, 1, 6, 5));
	parser (new mu::io::tokens::stream_end, mu::io::debugging::context (1, 7, 6, 1, 7, 6));
	EXPECT_EQ (result.results.size (), 1);
	EXPECT_EQ (result.results [0]->expressions.size (), 1);
	auto e1 (result.results [0]->expressions [0]);
	EXPECT_EQ (e1->values.empty (), true);
	EXPECT_EQ (e1->individual_names.empty (), true);
	EXPECT_EQ (e1->full_name->string, std::wstring (L"t1"));
	EXPECT_EQ (e1->context, mu::io::debugging::context (1, 1, 0, 1, 6, 5));
}

TEST (io_test, parser6)
{
	mu::io_test::parser_result result;
	auto errors (new (GC) mu::core::errors::error_list);
	mu::io::parser::parser parser (errors, boost::bind (&mu::io_test::parser_result::operator(), &result, _1));
	parser (new mu::io::tokens::left_square, mu::io::debugging::context (1, 1, 0, 1, 1, 0));
	parser (new mu::io::tokens::left_square, mu::io::debugging::context (1, 2, 1, 1, 2, 1));
	parser (new mu::io::tokens::right_square, mu::io::debugging::context (1, 3, 2, 1, 3, 2));
	parser (new mu::io::tokens::right_square, mu::io::debugging::context (1, 4, 3, 1, 4, 3));
	parser (new mu::io::tokens::stream_end, mu::io::debugging::context (1, 5, 4, 1, 5, 4));
	EXPECT_EQ (result.results.size (), 1);
	EXPECT_EQ (result.results [0]->expressions.size (), 1);
	auto e1 (result.results [0]->expressions [0]);
	EXPECT_EQ (e1->values.size (), 1);
	EXPECT_EQ (e1->individual_names.empty (), true);
	EXPECT_EQ (e1->full_name->string.empty (), true);
	EXPECT_EQ (e1->context, mu::io::debugging::context (1, 1, 0, 1, 4, 3));
	auto e2 (dynamic_cast <mu::io::ast::expression *> (e1->values [0]));
	EXPECT_EQ (e2->values.empty (), true);
	EXPECT_EQ (e2->individual_names.empty (), true);
	EXPECT_EQ (e2->full_name->string.empty (), true);
	EXPECT_EQ (e2->context, mu::io::debugging::context (1, 2, 1, 1, 3, 2));
}