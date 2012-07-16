#pragma once

#include <mu/io/tokens/token.h>

#include <string>

namespace mu
{
	namespace io
	{
		namespace tokens
		{
			class stream_end : public mu::io::tokens::token
			{
			public:
				mu::string token_name () override;
				void operator () (mu::io::tokens::visitor * visitor_a) override;
			};
		}
	}
}