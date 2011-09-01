#pragma once

#include <vector>

namespace lambda_p
{
	namespace core
	{
		class routine;
		class node;
		class result_ref;
		class result;
		class data;
		class statement
		{
			friend class ::lambda_p::core::result_ref;
			friend class ::lambda_p::core::data;
			friend class ::lambda_p::core::result;
		public:
			statement (::lambda_p::core::routine * routine_a, size_t index_a);
			~statement (void);
            void validate (::std::iostream & problems);
		private:
            ::lambda_p::core::routine * routine;
            size_t index;
            ::std::vector < ::lambda_p::core::node *> arguments;
		};
	}
}
