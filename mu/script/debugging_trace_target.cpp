#include <mu/script/debugging_trace_target.h>

#include <mu/script/frame.h>
#include <mu/script/debugging_trace_types.h>
#include <mu/script/context.h>
#include <mu/script/values_operation.h>
#include <mu/script/debugging_trace_error.h>

mu::script::debugging::trace_target::trace_target (mu::core::errors::error_target * target_a, mu::script::context & context_a)
	: target (target_a),
	context (context_a)
{
}

void mu::script::debugging::trace_target::operator () (mu::core::errors::error * error)
{
	//mu::script::frame frame (context);
	//context.push (boost::make_shared <mu::script::debugging::trace_types> ());
	//auto valid (context ());
	//assert (valid);
	//assert (context.working_size () == 1);
	//assert (boost::dynamic_pointer_cast <mu::script::values::operation> (context.working (0)));
	//auto values (boost::static_pointer_cast <mu::script::values::operation> (context.working (0)));
	//(*target) (boost::make_shared <mu::script::debugging::trace_error> (values, error));
	(*target) (error);
}

bool mu::script::debugging::trace_target::operator () ()
{
	auto result ((*target) ());
	return result;
}

void mu::script::debugging::trace_target::print (mu::ostream & target_a)
{
	target->print (target_a);
}