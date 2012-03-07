#include "lexer_result.h"

#include <mu/io/tokens/token.h>

#include <iostream>

void mu::io_test::lexer_result::operator () (mu::io::tokens::token * token_a, mu::core::context context_a)
{
	results.push_back (std::pair <mu::io::tokens::token *, mu::core::context> (token_a, context_a));
}

void mu::io_test::lexer_result::print (std::wostream & target)
{
    for (auto i (results.begin ()), j (results.end ()); i != j; ++i)
    {
        target << i->first->token_name ();
        target << L' ';
    }
}