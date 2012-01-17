#include "link_modules.h"

#include <lambda_p/errors/error_list.h>
#include <lambda_p_llvm/module/node.h>
#include <lambda_p_llvm/linker/link_modules.h>

#include <llvm/LLVMContext.h>
#include <llvm/Function.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Module.h>

#include <vector>

#include <boost/shared_ptr.hpp>

void lambda_p_llvm_test::linker::link_modules::run ()
{
	run_1 ();
}

void lambda_p_llvm_test::linker::link_modules::run_1 ()
{
	llvm::LLVMContext context;
	std::vector <llvm::Type *> types;
	auto module (boost::shared_ptr <lambda_p_llvm::module::node> (new lambda_p_llvm::module::node (new llvm::Module (llvm::StringRef ("test1"), context))));	
	auto module1 (boost::shared_ptr <lambda_p_llvm::module::node> (new lambda_p_llvm::module::node (new llvm::Module (llvm::StringRef ("test1"), context))));	
	auto function1 (llvm::Function::Create (llvm::FunctionType::get (llvm::Type::getVoidTy (context), types, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "a", module1->module));
	auto module2 (boost::shared_ptr <lambda_p_llvm::module::node> (new lambda_p_llvm::module::node (new llvm::Module (llvm::StringRef ("test2"), context))));	
	auto function2 (llvm::Function::Create (llvm::FunctionType::get (llvm::Type::getVoidTy (context), types, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "b", module2->module));
	lambda_p_llvm::linker::link_modules link_modules;
	boost::shared_ptr <lambda_p::errors::error_list> errors (new lambda_p::errors::error_list);
	std::vector <boost::shared_ptr <lambda_p::node>> args1;
	std::vector <boost::shared_ptr <lambda_p::node>> res1;
	args1.push_back (module);
	args1.push_back (module1);
	link_modules.perform (errors, args1, res1);
	assert (errors->errors.empty ());
	std::vector <boost::shared_ptr <lambda_p::node>> args2;
	std::vector <boost::shared_ptr <lambda_p::node>> res2;
	args2.push_back (module);
	args2.push_back (module2);
	link_modules.perform (errors, args2, res2);
	assert (errors->errors.empty ());
	assert (module->module->getFunction ("a") != nullptr);
	assert (module->module->getFunction ("b") != nullptr);
}