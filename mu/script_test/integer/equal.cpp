#include "equal.h"

#include <mu/core/errors/error_list.h>
#include <mu/script/integer/equal.h>
#include <mu/script/integer/node.h>
#include <mu/script/bool_c/node.h>

void mu::script_test::integer::equal::run ()
{
	run_1 ();
	run_2 ();
	run_3 ();
}

void mu::script_test::integer::equal::run_1 ()
{
	mu::script::integer::equal equal;
	auto errors (boost::shared_ptr <mu::core::errors::error_list> (new mu::core::errors::error_list));
	std::vector <boost::shared_ptr <mu::core::node>> arguments;
	std::vector <boost::shared_ptr <mu::core::node>> results;
	arguments.push_back (boost::shared_ptr <mu::core::node> (new mu::script::integer::node (3)));
	arguments.push_back (boost::shared_ptr <mu::core::node> (new mu::script::integer::node (5)));
	equal (errors, arguments, results);
	assert (errors->errors.empty ());
	assert (results.size () == 1);
	auto result (boost::dynamic_pointer_cast <mu::script::bool_c::node> (results [0]));
	assert (result.get () != nullptr);
	assert (result->value == false);
}

void mu::script_test::integer::equal::run_2 ()
{
	mu::script::integer::equal equal;
	auto errors (boost::shared_ptr <mu::core::errors::error_list> (new mu::core::errors::error_list));
	std::vector <boost::shared_ptr <mu::core::node>> arguments;
	std::vector <boost::shared_ptr <mu::core::node>> results;
	arguments.push_back (boost::shared_ptr <mu::core::node> (new mu::script::integer::node (3)));
	arguments.push_back (boost::shared_ptr <mu::core::node> (new mu::script::integer::node (3)));
	equal (errors, arguments, results);
	assert (errors->errors.empty ());
	assert (results.size () == 1);
	auto result (boost::dynamic_pointer_cast <mu::script::bool_c::node> (results [0]));
	assert (result.get () != nullptr);
	assert (result->value == true);
}

void mu::script_test::integer::equal::run_3 ()
{
	mu::script::integer::equal equal;
	auto errors (boost::shared_ptr <mu::core::errors::error_list> (new mu::core::errors::error_list));
	std::vector <boost::shared_ptr <mu::core::node>> arguments;
	std::vector <boost::shared_ptr <mu::core::node>> results;
	arguments.push_back (boost::shared_ptr <mu::core::node> (new mu::script::integer::node (3)));
	arguments.push_back (boost::shared_ptr <mu::core::node> (new mu::core::node));
	equal (errors, arguments, results);
	assert (!errors->errors.empty ());
}