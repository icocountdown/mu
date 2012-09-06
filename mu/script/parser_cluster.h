#pragma once

#include <mu/script/parser_state.h>
#include <mu/io/tokens_visitor.h>
#include <mu/io/analyzer_name_map.h>

namespace mu
{
    namespace script
    {
        namespace ast
        {
            class cluster;
        }
        namespace parser
        {
            class parser;
            class cluster : public mu::script::parser::state, public mu::io::tokens::visitor
            {
            public:
                cluster (mu::script::parser::parser & parser_a);
                mu::script::parser::parser & parser;
                void operator () (mu::io::tokens::token const & token_a) override;
                mu::io::analyzer::name_map map;
                mu::script::ast::cluster * cluster_m;
				void operator () (mu::io::tokens::divider const & token) override;
				void operator () (mu::io::tokens::identifier const & token) override;
				void operator () (mu::io::tokens::left_square const & token) override;
				void operator () (mu::io::tokens::right_square const & token) override;
				void operator () (mu::io::tokens::stream_end const & token) override;
                void operator () (mu::io::tokens::value const & token) override;
            };
        }
    }
}