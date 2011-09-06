#include <lambda_p/serialization/parser/routine_parameter.h>

lambda_p::serialization::parser::routine_parameter::routine_parameter(::std::wstring routine_name_a)
	: routine_name (routine_name_a),
	parameter_count (0),
	positions (new ::std::map < ::lambda_p::serialization::parser::result_reference, ::lambda_p::serialization::parser::result_position>)
{
}

lambda_p::serialization::parser::routine_parameter::~routine_parameter(void)
{
}

::lambda_p::serialization::parser::state_id lambda_p::serialization::parser::routine_parameter::state_type ()
{
	return ::lambda_p::serialization::parser::state_routine_parameter;
}