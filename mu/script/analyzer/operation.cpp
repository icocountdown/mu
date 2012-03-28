#include <mu/script/analyzer/operation.h>

#include <mu/script/extensions/node.h>
#include <mu/io/ast/cluster.h>
#include <mu/io/analyzer/analyzer.h>
#include <mu/io/analyzer/extensions/global.h>
#include <mu/script/extensions/node.h>
#include <mu/io/analyzer/extensions/extensions.h>
#include <mu/io/ast/expression.h>
#include <mu/io/ast/end.h>
#include <mu/script/check.h>
#include <mu/io/debugging/stream.h>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

bool mu::script::analyzer::operation::operator () (mu::script::context & context_a)
{
	bool result (mu::script::check <mu::script::extensions::node, mu::io::ast::cluster> () (context_a));
	if (result)
	{
		auto extensions (boost::static_pointer_cast <mu::script::extensions::node> (context_a.parameters (0)));
		auto ast (boost::static_pointer_cast <mu::io::ast::cluster> (context_a.parameters (1)));
		auto result (core (context_a, extensions, ast));
	}
	return result;
}

boost::shared_ptr <mu::core::cluster> mu::script::analyzer::operation::core (mu::script::context & context_a, boost::shared_ptr <mu::script::extensions::node> extensions, boost::shared_ptr <mu::io::ast::cluster> ast)
{
	boost::shared_ptr <mu::core::cluster> result;
	mu::io::analyzer::analyzer analyzer (boost::bind (&mu::script::analyzer::operation::build, this, &result, _1), context_a.errors.target, extensions->extensions, boost::make_shared <mu::io::debugging::stream> ());
	analyzer.input (ast);
	return result;
}

void mu::script::analyzer::operation::build (boost::shared_ptr <mu::core::cluster> * result_a, boost::shared_ptr <mu::core::cluster> cluster_a)
{
	*result_a = cluster_a;
}