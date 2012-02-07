#pragma once

#include <lambda_p_llvm/value/node.h>

namespace llvm
{
	class Argument;
}
namespace lambda_p_llvm
{
	namespace argument
	{
		class node : public lambda_p_llvm::value::node
		{
		public:
			node (llvm::Argument * argument_a, boost::shared_ptr <lambda_p_llvm::type::node> type_a);
			llvm::Argument * argument ();
		};
	}
}

