#pragma once

#include <mu/llvm_/constant_node.h>

namespace llvm
{
	class ConstantInt;
}
namespace mu
{
	namespace llvm_
	{
		namespace constant_int
		{
			class node : public mu::llvm_::constant::node
			{
			public:
				node (llvm::ConstantInt * constant_int_a, mu::llvm_::type::node * type_a);
				mu::string name () override;
				llvm::ConstantInt * constant_int ();
			};
		}
	}
}