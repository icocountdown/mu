#include <mu/script/astring/truncate.h>

#include <mu/script/string/node.h>
#include <mu/script/astring/node.h>
#include <mu/script/check.h>

#include <gc_cpp.h>

bool mu::script::astring::truncate::operator () (mu::script::context & context_a)
{
	bool result (mu::script::check <mu::script::string::node> (context_a));
	if (result)
	{
		auto one (static_cast <mu::script::string::node *> (context_a.parameters (0)));
		std::string string (one->string.begin (), one->string.end ());
		context_a.push (new (GC) mu::script::astring::node (string));
	}
	return result;
}