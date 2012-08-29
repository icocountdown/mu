#include <mu/core/errors/error_list.h>
#include <mu/script/loads_operation.h>
#include <mu/script/cluster_node.h>
#include <mu/script/string_node.h>
#include <mu/script/context.h>
#include <mu/io/keywording_extensions.h>

#include <gtest/gtest.h>

#include <gc_cpp.h>

TEST (script_test, loads1)
{
	mu::core::errors::error_list errors;
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::script::loads::operation);
	ctx.push (new (GC) mu::script::string::node (mu::string (U"source_test.mu")));
	auto valid (ctx ());
    ASSERT_TRUE (valid);
	ASSERT_TRUE (ctx.working_size () == 1);
	auto cluster (dynamic_cast <mu::script::cluster::node *> (ctx.working (0)));
	ASSERT_TRUE (cluster != nullptr);
	auto a (cluster->routines.find (mu::string (U"a")));
    ASSERT_TRUE (a != cluster->routines.end ());
	auto b (cluster->routines.find (mu::string (U"b")));
	ASSERT_TRUE (b != cluster->routines.end ());
}