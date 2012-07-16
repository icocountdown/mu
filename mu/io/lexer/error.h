#pragma once

#include <mu/io/lexer/state.h>

#include <string>

namespace mu
{
	namespace io
	{
		namespace lexer
		{
			class error : public mu::io::lexer::state
			{
			public:
				void lex (char32_t character) override;
			};
		}
	}
}