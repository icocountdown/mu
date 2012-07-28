#include <mu/script/extensions/merge.h>

#include <mu/core/errors/error_target.h>
#include <mu/script/extensions/node.h>
#include <mu/script/string/node.h>
#include <mu/script/check.h>

#include <sstream>

#include <assert.h>

bool mu::script::extensions::merge::operator () (mu::script::context & context_a)
{
	bool result (mu::script::check <mu::script::extensions::node, mu::script::string::node, mu::script::extensions::node> () (context_a));
	if (result)
	{
		auto one (static_cast <mu::script::extensions::node *> (context_a.parameters (0)));
		auto two (static_cast <mu::script::string::node *> (context_a.parameters (1)));
		auto three (static_cast <mu::script::extensions::node *> (context_a.parameters (2)));
        assert (false);
/*		for (auto i (three->extensions->extensions_m.begin ()), j (three->extensions->extensions_m.end ()); i != j; ++i)
		{
			mu::string name (two->string.begin (), two->string.end ());
			name.append (i->first);
			auto existing ((*one->extensions) [name]);
			if (existing == nullptr)
			{
				(*one->extensions) [name] = i->second;
			}
			else
			{
				mu::stringstream message;
				message << L"Extensions already contains extension named: ";
				message << i->first;
				context_a.errors (message.str ());
				result = false;
			}
		}*/
	}
	return result;
}

mu::string mu::script::extensions::merge::name ()
{
	return mu::string (U"mu::script::extensions::merge");
}