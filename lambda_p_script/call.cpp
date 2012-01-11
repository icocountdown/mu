#include "call.h"

#include <lambda_p_script/node.h>
#include <lambda_p_script/operation.h>
#include <lambda_p/errors/error_target.h>
#include <lambda_p_script/context.h>

lambda_p_script::call::call (size_t results_a)
	: results (results_a)
{
}

void lambda_p_script::call::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, lambda_p_script::context & context)
{
	std::vector <boost::shared_ptr <lambda_p::node>> arguments_l;
	for (auto i (arguments.begin ()), j (arguments.end ()); i != j; ++i)
	{
		(*(*i)) (errors_a, context, arguments_l);
	}
	std::vector <boost::shared_ptr <lambda_p::node>> results_l;
	if (arguments_l.size () > 0)
	{
		auto operation (boost::dynamic_pointer_cast <lambda_p_script::operation> (arguments_l [0]));
		if (operation.get () != nullptr)
		{
			auto segment (lambda_p_script::segment <boost::shared_ptr <lambda_p::node>> (1, arguments_l.size (), arguments_l));
			operation->perform (errors_a, arguments_l, results_l);
			std::vector <boost::shared_ptr <lambda_p::node>> & target (context.nodes [results]);
			assert (target.empty () && L"Destination has already been assigned");
			target.assign (results_l.begin (), results_l.end ());
		}
		else
		{
			(*errors_a) (L"First argument to call is not an operation");
		}
	}
	else
	{
		(*errors_a) (L"Call has no arguments");
	}
}