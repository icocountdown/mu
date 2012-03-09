#pragma once

#include <mu/script/operation.h>

namespace mu
{
	namespace llvm_
	{
		namespace module
		{
			class get_function : public mu::script::operation
			{
			public:
				void operator () (mu::script::context & context_a) override;
				std::wstring name () override;
			};
		}
	}
}
