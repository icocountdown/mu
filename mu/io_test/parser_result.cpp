#include <mu/io_test/parser_result.h>

void mu::io_test::parser_result::operator () (boost::shared_ptr <mu::io::ast::cluster> result_a)
{
	results.push_back (result_a);
}