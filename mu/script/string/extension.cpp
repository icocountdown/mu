#include <mu/script/string/extension.h>

#include <mu/io/analyzer/expression.h>
#include <mu/io/analyzer/routine.h>
#include <mu/io/analyzer/analyzer.h>
#include <mu/io/ast/expression.h>
#include <mu/core/errors/error_target.h>
#include <mu/io/ast/identifier.h>
#include <mu/core/expression.h>
#include <mu/script/string/node.h>
#include <mu/core/errors/string_error.h>

#include <sstream>

#include <gc_cpp.h>

mu::script::string::extension::extension (mu::io::keywording::keywording & keywording_a)
: keywording (keywording_a)
{
}

void mu::script::string::extension::operator () (mu::io::tokens::token * token_a, mu::io::debugging::context context_a)
{
    assert (false);/*
    assert (remaining.empty ());
	auto data_position (expression_a.position + 1);
	if (expression_a.expression_m->values.size () > data_position)
	{
		expression_a.position = data_position;
		auto data (dynamic_cast <mu::io::ast::identifier *> (expression_a.expression_m->values [data_position]));
		if (data != nullptr)
		{
			expression_a.self->dependencies.push_back (new (GC) mu::script::string::node (data->string));
		}
		else
		{
			errors_a (U"String extension requires its argument to be an identifier");
		}
	}
	else
	{
		errors_a (U"String extension requires one argument");
	}*/
}