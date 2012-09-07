#pragma once

#include <mu/script/operation.h>

namespace mu
{
	namespace script
	{
		namespace print
		{
			class operation : public mu::script::operation
			{
			public:
				bool operator () (mu::script::context & context_a) override;
			};
		}
	}
}
