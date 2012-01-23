#pragma once

#include <lambda_p_llvm/operation.h>

namespace lambda_p
{
	class routine;
}
namespace lambda_p_llvm
{
	namespace lambda
	{
		class operation : public lambda_p_llvm::operation
		{
		public:
			operation (boost::shared_ptr <lambda_p::routine> routine_a);
			void operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, llvm::BasicBlock * & context_a, lambda_p_script::segment <boost::shared_ptr <lambda_p::node>> parameters_a, std::vector <boost::shared_ptr <lambda_p::node>> & results_a) override;
			boost::shared_ptr <lambda_p::routine> routine;
		};
	}
}
