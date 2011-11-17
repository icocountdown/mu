#pragma once

#include <lambda_p/binder/binder.h>
#include <lambda_p/binder/list.h>

namespace lambda_p
{
	namespace core
	{
		class routine;
	}
}
namespace lambda_p_kernel
{
	// Applies nodes to the routine's surface and does all the work of mapping surface index to the corresponding routine declarations
	// This is a one-shot apply and doesn't do partial application
	class apply : public lambda_p::binder::binder
	{
	public:
		apply (void);
		void bind (lambda_p::core::statement * statement, lambda_p::binder::list & nodes, lambda_p::errors::error_list & problems) override;
		void core (boost::shared_ptr <lambda_p::core::routine> routine, lambda_p::binder::list & nodes_l, lambda_p::errors::error_list & problems, lambda_p::binder::list & declarations);
		std::wstring binder_name () override;
	};
}