#include <mu/script/extensions_merge_package.h>

#include <mu/core/errors/error_target.h>
#include <mu/script/extensions_node.h>
#include <mu/script/string_node.h>
#include <mu/script/package_node.h>
#include <mu/core/check.h>
#include <mu/io/keywording_extensions.h>
#include <mu/script/context.h>

#include <sstream>

#include <gc_cpp.h>

bool mu::script::extensions::merge_package::operator () (mu::script::context & context_a)
{
	bool result (mu::core::check <mu::script::extensions::node, mu::script::string::node, mu::script::package::node> (context_a));
	if (result)
	{
		auto one (static_cast <mu::script::extensions::node *> (context_a.parameters (0)));
		auto two (static_cast <mu::script::string::node *> (context_a.parameters (1)));
		auto three (static_cast <mu::script::package::node *> (context_a.parameters (2)));
		for (auto i (three->items.begin ()), j (three->items.end ()); i != j; ++i)
		{
			mu::string name (two->string.begin (), two->string.end ());
			name.append (i->first);
			auto existing ((*one->extensions) (name));
			if (boost::get <0> (existing).empty ())
			{
				(*one->extensions) (name, i->second);
			}
			else
			{
				mu::stringstream message;
				message << L"Extensions already has an extension named: ";
				message << name;
				context_a.errors (message.str ());
				result = false;
			}
		}
	}
	return result;
}