#pragma once

namespace lambda_p_serialization
{
	namespace tokens
	{
		class token;
	}
	namespace parser
	{
		class state
		{
		public:
			virtual void parse (lambda_p_serialization::tokens::token * token) = 0;
		};
	}
}

