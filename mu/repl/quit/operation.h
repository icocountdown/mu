#pragma once

#include <mu/script/operation.h>

namespace mu
{
	namespace repl
	{
		class repl;
		namespace quit
		{
			class operation : public mu::script::operation
			{
			public:
				operation (mu::repl::repl & repl_a);
				bool operator () (mu::script::context & context_a) override;
				mu::repl::repl & repl;
			};
		}
	}
}
