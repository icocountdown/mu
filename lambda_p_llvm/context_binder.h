#pragma once

#include <lambda_p/binder/node_binder.h>

namespace lambda_p_llvm
{
	class context_binder : public ::lambda_p::binder::node_binder
	{
	public:
		context_binder(void);
		~context_binder(void);
		void bind (::lambda_p::core::statement * statement, ::std::map < size_t, ::boost::shared_ptr < ::lambda_p::binder::node_instance> > & instances, ::std::vector < ::boost::shared_ptr < ::lambda_p::errors::error> > & problems);
		::std::wstring binder_name ();
	};
}

