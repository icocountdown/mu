#pragma once

#include <mu/io/debugging/context.h>

namespace mu
{
    namespace io
    {
        namespace tokens
        {
            class token;
        }
        namespace keywording
        {
            class state
            {
            public:
                virtual void operator () (mu::io::tokens::token * token_a, mu::io::debugging::context context_a) = 0;
            };
        }
    }
}