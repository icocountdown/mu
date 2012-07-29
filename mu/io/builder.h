#pragma once

#include <mu/io/lexer/lexer.h>
#include <mu/io/parser/parser.h>
#include <mu/io/analyzer/analyzer.h>
#include <mu/core/errors/error_list.h>
#include <mu/io/keywording/keywording.h>

#include <vector>

namespace mu
{
	namespace io
	{
		namespace ast
		{
			class node;
			class cluster;
		}
		namespace debugging
		{
			class mapping;
		}
        namespace lexer
        {
            class context;
        }
        namespace keywording
        {
            class extensions;
        }
		class builder
		{
		public:
			builder ();
			builder (mu::io::keywording::extensions * extensions_a);
			void operator () (mu::io::lexer::context const & context_a);
			mu::core::errors::error_list errors;
			mu::io::analyzer::analyzer analyzer;
			mu::io::parser::parser parser;
            mu::io::keywording::keywording keywording;
			mu::io::lexer::lexer lexer;
			void add (mu::core::cluster *);
			mu::core::cluster * cluster;
		};
	}
}
