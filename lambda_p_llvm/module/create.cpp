#include "create.h"

#include <lambda_p_llvm/context/node.h>
#include <lambda_p_llvm/module/node.h>

#include <llvm/Module.h>

void lambda_p_llvm::module::create::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, lambda_p::segment <boost::shared_ptr <lambda_p::node>> parameters, std::vector <boost::shared_ptr <lambda_p::node>> & results)
{
	auto one (boost::dynamic_pointer_cast <lambda_p_llvm::context::node> (parameters [0]));
	if (one.get () != nullptr)
	{
		results.push_back (boost::shared_ptr <lambda_p::node> (new lambda_p_llvm::module::node (new llvm::Module (llvm::StringRef (), *one->context))));
	}
	else
	{
		invalid_type (errors_a, parameters [0], 0);
	}
}

size_t lambda_p_llvm::module::create::count ()
{
	return 1;
}

std::wstring lambda_p_llvm::module::create::name ()
{
	return std::wstring (L"lambda_p_llvm::module::create");
}