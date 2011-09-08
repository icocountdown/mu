#include <lambda_p/core/declaration.h>

#include <iostream>

#include <lambda_p/core/routine.h>

lambda_p::core::declaration::declaration(::lambda_p::core::routine * routine_a, size_t self_statement_a, size_t self_argument_a)
	: routine (routine_a),
	self_statement (self_statement_a),
	self_argument (self_argument_a)
{
}

lambda_p::core::declaration::~declaration(void)
{
}

void lambda_p::core::declaration::validate (::std::iostream & problems) const
{
	validate_argument ("declaration: ", routine, self_statement, self_argument, problems);
}

::lambda_p::core::node_id lambda_p::core::declaration::node_type () const
{
	return ::lambda_p::core::node_result;
}