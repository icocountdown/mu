#include "extension.h"

#include <lambda_p_io/analyzer/expression.h>
#include <lambda_p_io/analyzer/routine.h>
#include <lambda_p_io/analyzer/analyzer.h>
#include <lambda_p_io/ast/expression.h>
#include <lambda_p/errors/error_target.h>
#include <lambda_p_io/ast/identifier.h>
#include <lambda_p/expression.h>
#include <lambda_p_script/integer/node.h>

#include <sstream>

#include <boost/make_shared.hpp>

void lambda_p_script::integer::extension::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, lambda_p_io::analyzer::expression & expression_a)
{
	auto data_position (expression_a.position + 1);
	expression_a.position = data_position;
	if (expression_a.expression_m->values.size () > data_position)
	{
		auto data (boost::dynamic_pointer_cast <lambda_p_io::ast::identifier> (expression_a.expression_m->values [data_position]));
		if (data.get () != nullptr)
		{
			auto string (data->string);
			auto result (core (errors_a, string));
			if (result.get () != nullptr)
			{
				expression_a.self->dependencies.push_back (result);
			}
		}
		else
		{
			(*errors_a) (L"Number extension requires its argument to be an identifier");
		}
	}
	else
	{
		(*errors_a) (L"Number extension requires one argument");
	}
}

boost::shared_ptr <lambda_p_script::integer::node> lambda_p_script::integer::core (boost::shared_ptr <lambda_p::errors::error_target> errors_a, std::wstring & string)
{
	boost::shared_ptr <lambda_p_script::integer::node> result;
	int base (0);
	wchar_t base_char (string [0]);
	switch (base_char)
	{
		case L'h':
			base = 16;
			break;
		case L'd':
			base = 10;
			break;
		case L'o':
			base = 8;
			break;
		case L'b':
			base = 2;
			break;
		default:
			std::wstringstream message;
			message << L"Unexpected base prefix: ";
			message << base_char;
			message << L" when trying to parse number: ";
			message << string;
			(*errors_a) (message.str ());
		break;
	}
	if (base != 0)
	{
		wchar_t * next;
		wchar_t const * string_l (string.c_str () + 1);
		errno = 0;
		unsigned long number = std::wcstol (string_l, &next, base);
		if (errno == ERANGE)
		{
			std::wstringstream message;
			message << L"Overflow while parsing: ";
			message << string;
			(*errors_a) (message.str ());
		}
		else
		{
			result = boost::make_shared <lambda_p_script::integer::node> (number);
		}
	}
	return result;
}