#pragma once

namespace lambda_p_io
{
	namespace lexer
	{
		class state
		{
		public:
			virtual void lex (wchar_t character) = 0;
		};
	}
}