#pragma once

#include <lambda_p/binder/binder.h>

namespace lambda_p_repl
{
	class file_routine : public ::lambda_p::binder::binder
	{
	public:
		file_routine(void);
		void bind (::lambda_p::core::statement * statement, ::lambda_p::binder::routine_instances & instances, ::std::vector < ::boost::shared_ptr < ::lambda_p::errors::error> > & problems);
		::std::wstring binder_name ();
	};
}
