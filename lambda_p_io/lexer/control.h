#pragma once

#include <core/position.h>
#include <lambda_p_io/lexer/state.h>

namespace lambda_p_io
{
	namespace lexer
	{
		class lexer;
		class control : public lambda_p_io::lexer::state
		{
		public:
			control (lambda_p_io::lexer::lexer & lexer_a, mu::core::position first_a);
			void lex (wchar_t character) override;
			mu::core::position first;
			lambda_p_io::lexer::lexer & lexer;
		};
	}
}

