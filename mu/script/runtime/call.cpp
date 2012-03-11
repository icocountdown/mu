#include <mu/script/runtime/call.h>

#include <mu/script/runtime/node.h>
#include <mu/script/operation.h>
#include <mu/core/errors/error_context.h>
#include <mu/script/runtime/frame.h>
#include <mu/core/routine.h>
#include <mu/script/runtime/routine.h>
#include <mu/script/context.h>

#include <boost/make_shared.hpp>

#include <sstream>

mu::script::runtime::call::call (size_t results_a)
	: results (results_a)
{
}

mu::script::runtime::call::call (size_t results_a, mu::core::context context_a)
	: context (context_a),
	results (results_a)
{
}

void mu::script::runtime::call::operator () (mu::script::context & context_a, mu::script::runtime::frame & frame_a)
{
	std::vector <boost::shared_ptr <mu::core::node>> arguments_l;
	auto errors_l (boost::make_shared <mu::core::errors::error_context> (context_a.errors, context));
	for (auto i (arguments.begin ()), j (arguments.end ()); i != j; ++i)
	{
		(*(*i)) (context_a.errors, frame_a, arguments_l);
	}
	if (arguments_l.size () > 0)
	{
		auto operation (boost::dynamic_pointer_cast <mu::script::operation> (arguments_l [0]));
		if (operation.get () != nullptr)
		{
			(*this) (errors_l, context_a.stack, operation, arguments_l, frame_a);
		}
		else
		{
			std::wstringstream message;
			message << L"First argument to call is not an operation: ";
			message << arguments_l [0]->name ();
			(*errors_l) (message.str ());
		}
	}
	else
	{
		(*errors_l) (L"Call has no arguments");
	}
}

void mu::script::runtime::call::operator () (boost::shared_ptr <mu::core::errors::error_target> errors_a, std::vector <std::type_info const *> stack_a, boost::shared_ptr <mu::script::operation> operation_a, std::vector <boost::shared_ptr <mu::core::node>> & arguments_a, mu::script::runtime::frame & frame_a)
{
	std::vector <boost::shared_ptr <mu::core::node>> results_l;
	auto segment (mu::core::segment <boost::shared_ptr <mu::core::node>> (1, arguments_a));
    auto ctx (mu::script::context (errors_a, segment, results_l, stack_a));
	(*operation_a) (ctx);
	std::vector <boost::shared_ptr <mu::core::node>> & target (frame_a.nodes [results]);
	assert (target.empty () && L"Destination has already been assigned");
	target.assign (results_l.begin (), results_l.end ());
}