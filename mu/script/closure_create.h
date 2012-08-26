#pragma once

#include <mu/script/operation.h>

namespace mu
{
	namespace script
	{
		namespace closure
		{
			class create : public mu::script::operation
			{
			public:
				bool operator () (mu::script::context & context_a) override;
				mu::string name () override;
			};
		}
	}
}