#pragma once

#include <mu/script/operation.h>

namespace mu
{
	namespace llvm_
	{
		namespace value
		{
			class get_context : public mu::script::operation
			{
			public:
				void operator () (mu::script::context & context_a) override;
			};
		}
	}
}
