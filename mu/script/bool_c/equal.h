#pragma once

#include <mu/script/operation.h>

namespace mu
{
	namespace script
	{
		namespace bool_c
		{
			class equal : public mu::script::operation
			{
			public:
				void operator () (mu::script::context & context_a) override;
				std::wstring name () override;
			};
		}
	}
}
