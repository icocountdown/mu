#pragma once

#include <mu/script/operation.h>

namespace mu
{
	namespace llvm_
	{
		namespace function
		{
			class create : public mu::script::operation
			{
			public:
				void operator () (mu::script::context & context_a) override;
			};
		}
	}
}
