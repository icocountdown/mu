#pragma once

#include <mu/script/operation.h>

namespace mu
{
	namespace script
	{
		namespace extensions
		{
			class merge : public mu::script::operation
			{
			public:
				bool operator () (mu::script::context & context_a) override;
				mu::string name () override;
			};
		}
	}
}

