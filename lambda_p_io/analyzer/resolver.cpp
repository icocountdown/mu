#include "resolver.h"

#include <core/expression.h>

lambda_p_io::analyzer::resolver::resolver (boost::shared_ptr <mu::core::expression> unresolved_a, size_t position_a)
	: unresolved (unresolved_a),
	position (position_a)
{
	assert (unresolved_a->dependencies [position_a].get () == nullptr);
}

void lambda_p_io::analyzer::resolver::operator () (boost::shared_ptr <mu::core::node> node_a)
{
	assert (unresolved->dependencies [position].get () == nullptr);
	unresolved->dependencies [position] = node_a;
}