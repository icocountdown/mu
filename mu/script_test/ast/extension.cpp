#include <mu/script_test/ast/extension.h>

#include <mu/core/expression.h>
#include <mu/io/ast/expression.h>
#include <mu/io/ast/identifier.h>
#include <mu/io/ast/cluster.h>
#include <mu/io/analyzer/analyzer.h>
#include <mu/io/builder.h>
#include <mu/core/errors/error_list.h>
#include <mu/script/context.h>
#include <mu/io_test/analyzer_result.h>
#include <mu/core/routine.h>

#include <boost/bind.hpp>

void mu::script_test::ast::extension::run ()
{
	run_1 ();
	run_2 ();
	run_3 ();
}

void mu::script_test::ast::extension::run_1 ()
{
	mu::io::builder builder;
	mu::io::source source (boost::bind (&mu::io::lexer::lexer::operator(), &builder.lexer, _1));
	source (L"[~ .ast []]");
	source ();
	assert (builder.errors->errors.empty ());
	assert (builder.clusters.size () == 1);
	mu::io_test::analyzer_result result;
	mu::io::analyzer::analyzer (boost::bind (&mu::io_test::analyzer_result::operator(), &result, _1, _2), builder.errors);
	assert (result.clusters.size () == 1);
	auto cluster (result.clusters [0]);
	assert (cluster->routines.size () == 1);
	auto routine (cluster->routines [0]);
	mu::script::context ctx (mu::core::errors::errors (builder.errors));
	ctx.push (routine);
	auto valid (ctx ());
	assert (valid);
	assert (ctx.working_size () == 1);
	auto cl (boost::dynamic_pointer_cast <mu::io::ast::cluster> (ctx.working (0)));
	assert (cl.get () != nullptr);
	assert (cl->expressions.size () == 0);
}

void mu::script_test::ast::extension::run_2 ()
{
	mu::io::builder builder;
	mu::io::source source (boost::bind (&mu::io::lexer::lexer::operator(), &builder.lexer, _1));
	source (L"[~ .ast thing]");
	source ();
	assert (!builder.errors->errors.empty ());
	assert (builder.clusters.empty ());
}

void mu::script_test::ast::extension::run_3 ()
{
	mu::io::builder builder;
	mu::io::source source (boost::bind (&mu::io::lexer::lexer::operator(), &builder.lexer, _1));
	source (L"[~ .ast [[]]]");
	source ();
	assert (builder.errors->errors.empty ());
	assert (builder.clusters.size () == 1);
	mu::io_test::analyzer_result result;
	mu::io::analyzer::analyzer (boost::bind (&mu::io_test::analyzer_result::operator(), &result, _1, _2), builder.errors);
	assert (result.clusters.size () == 1);
	auto cluster (result.clusters [0]);
	assert (cluster->routines.size () == 1);
	auto routine (cluster->routines [0]);
	mu::script::context ctx (mu::core::errors::errors (builder.errors));
	ctx.push (routine);
	auto valid (ctx ());
	assert (valid);
	assert (ctx.working_size () == 1);
	auto cl (boost::dynamic_pointer_cast <mu::io::ast::cluster> (ctx.working (0)));
	assert (cl.get () != nullptr);
	assert (cl->expressions.size () == 1);
	auto expression (cl->expressions [0]);
}