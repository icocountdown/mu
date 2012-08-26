#include <mu/llvm_/basic_block_instruction_insert.h>
#include <mu/llvm_/basic_block_node.h>
#include <mu/llvm_/instruction_node.h>
#include <mu/script/context.h>
#include <mu/core/errors/error_list.h>
#include <mu/llvm_/instructions_add.h>
#include <mu/llvm_/integer_type_node.h>

#include <llvm/LLVMContext.h>
#include <llvm/BasicBlock.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Function.h>
#include <llvm/Instructions.h>
#include <llvm/Constants.h>

#include <gtest/gtest.h>

#include <gc_cpp.h>

TEST (llvm_test, instruction_insert1)
{
	llvm::LLVMContext ctx;
	auto bl (llvm::BasicBlock::Create (ctx));
	auto mod (new llvm::Module (llvm::StringRef (), ctx));
	auto function (llvm::Function::Create (llvm::FunctionType::get (llvm::Type::getVoidTy (ctx), false), llvm::GlobalValue::LinkageTypes::ExternalLinkage));
	mod->getFunctionList ().push_back (function);
	auto block (new (GC) mu::llvm_::basic_block::node (bl));
	function->getBasicBlockList ().push_back (block->block);
	mu::core::errors::error_list errors;
	mu::script::context context (errors);
	context.push (new (GC) mu::llvm_::basic_block::instruction_insert (block, new (GC) mu::llvm_::instructions::add));
	context.push (new (GC) mu::llvm_::value::node (llvm::ConstantInt::get (llvm::Type::getInt32Ty (ctx), 1, false), new (GC) mu::llvm_::integer_type::node (llvm::Type::getInt32Ty (ctx))));
	context.push (new (GC) mu::llvm_::value::node (llvm::ConstantInt::get (llvm::Type::getInt32Ty (ctx), 1, false), new (GC) mu::llvm_::integer_type::node (llvm::Type::getInt32Ty (ctx))));
	auto valid (context ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (context.working_size (), 1);
}