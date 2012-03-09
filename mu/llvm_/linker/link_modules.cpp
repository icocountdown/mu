#include "link_modules.h"

#include <mu/core/errors/error_target.h>
#include <mu/llvm_/module/node.h>
#include <mu/script/check.h>

#include <llvm/Linker.h>

void mu::llvm_::linker::link_modules::operator () (mu::script::context & context_a)
{
	if (mu::script::check <mu::llvm_::module::node, mu::llvm_::module::node> () (context_a))
	{
		auto one (boost::static_pointer_cast <mu::llvm_::module::node> (context_a.parameters [0]));
		auto two (boost::static_pointer_cast <mu::llvm_::module::node> (context_a.parameters [1]));
		std::string message;
		bool result (llvm::Linker::LinkModules (one->module, two->module, llvm::Linker::DestroySource, &message));
		if (result)
		{
			std::wstring message_l (message.begin (), message.end ());
			(*context_a.errors) (message_l);
		}
	}
}

std::wstring mu::llvm_::linker::link_modules::name ()
{
	return std::wstring (L"mu::llvm_::linker::link_modules");
}