#include <lambda_p/binder/data.h>

#include <lambda_p/core/routine.h>

lambda_p::binder::data::data(::std::wstring string_a)
	: string_m (string_a)
{
}

lambda_p::binder::data::~data(void)
{
}

::std::wstring lambda_p::binder::data::string () const
{
    return string_m;
}

::lambda_p::core::node_id lambda_p::binder::data::node_type () const
{
	return ::lambda_p::core::node_data;
}