#pragma once

#include <mu/core/node.h>

namespace llvm
{
	class Module;
}
namespace mu
{
	namespace llvm_
	{
		namespace module
		{
			class node : public mu::core::node
			{
			public:
				node (llvm::Module * module_a);
				mu::string debug () override;
				llvm::Module * module;
			};
		}
	}
}
