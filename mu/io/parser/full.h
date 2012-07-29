#pragma once

#include <mu/io/debugging/context.h>
#include <mu/io/tokens/visitor.h>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include <vector>

#include <gc_allocator.h>

namespace mu
{
	namespace io
	{
		namespace ast
		{
			class node;
			class expression;
			class identifier;
		}
		namespace parser
		{
			class parser;
			class target;
			class full : public mu::io::tokens::visitor
			{
			public:
				full (mu::io::parser::parser & parser_a, mu::io::parser::target & target_a, std::vector <mu::io::ast::node *, gc_allocator <mu::io::ast::node *>> values_a, std::vector <mu::io::ast::identifier *, gc_allocator <mu::io::ast::identifier *>> names_a, mu::io::debugging::context first_a);
				void operator () (mu::io::tokens::divider * token) override;
				void operator () (mu::io::tokens::identifier * token) override;
				void operator () (mu::io::tokens::left_square * token) override;
				void operator () (mu::io::tokens::right_square * token) override;
				void operator () (mu::io::tokens::stream_end * token) override;
				void operator () (mu::io::tokens::parameters * token) override;
				void operator () (mu::io::tokens::value * token) override;
				void unexpected_token (mu::io::tokens::token * token);
				mu::io::parser::parser & parser;
				mu::io::parser::target & target;
				std::vector <mu::io::ast::node *, gc_allocator <mu::io::ast::node *>> values;
				std::vector <mu::io::ast::identifier *, gc_allocator <mu::io::ast::identifier *>> names;
				mu::io::ast::identifier * full_name;			
				mu::io::debugging::context first;
			};
		}
	}
}
