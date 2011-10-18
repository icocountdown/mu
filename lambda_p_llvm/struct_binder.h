#pragma once

#include <lambda_p/binder/binder.h>

namespace lambda_p_llvm
{
	class generation_context;
	class struct_binder : public ::lambda_p::binder::binder
	{
	public:
		struct_binder (::lambda_p_llvm::generation_context & context_a);
		void bind (::lambda_p::core::statement * statement, ::lambda_p::binder::routine_instances & instances, ::std::vector < ::boost::shared_ptr < ::lambda_p::errors::error> > & problems);
		::std::wstring binder_name ();
		::lambda_p_llvm::generation_context & context;
	};
}
