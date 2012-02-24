#include "global.h"

#include <lambda_p_io/analyzer/expression.h>
#include <core/expression.h>

lambda_p_io::analyzer::extensions::global::global (boost::shared_ptr <mu::core::node> node_a)
	: node (node_a)
{
}

void lambda_p_io::analyzer::extensions::global::operator () (boost::shared_ptr <mu::core::errors::error_target> errors_a, lambda_p_io::analyzer::expression & expression_a)
{
	expression_a.self->dependencies.push_back (node);
}