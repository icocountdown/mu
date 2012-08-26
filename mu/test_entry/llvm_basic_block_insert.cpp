#include <mu/llvm_/basic_block_node.h>
#include <mu/llvm_/instruction_node.h>
#include <mu/llvm_/basic_block_insert.h>
#include <mu/core/errors/error_list.h>
#include <mu/llvm_/pointer_type_node.h>
#include <mu/llvm_/integer_type_node.h>
#include <mu/script/context.h>

#include <llvm/LLVMContext.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instructions.h>
#include <llvm/Function.h>
#include <llvm/Module.h>

#include <gtest/gtest.h>

#include <gc_cpp.h>

TEST (llvm_test, insert1)
{
	llvm::LLVMContext context;
	auto module (new llvm::Module (llvm::StringRef (), context));
	auto function (llvm::Function::Create (llvm::FunctionType::get (llvm::Type::getVoidTy (context), false), llvm::GlobalValue::ExternalLinkage));
	module->getFunctionList ().push_back (function);
	auto block (new (GC) mu::llvm_::basic_block::node (llvm::BasicBlock::Create (context)));
	function->getBasicBlockList ().push_back (block->block);
	auto instruction (new (GC) mu::llvm_::instruction::node (new llvm::AllocaInst (llvm::Type::getInt1Ty (context)), new (GC) mu::llvm_::pointer_type::node (new (GC) mu::llvm_::integer_type::node (llvm::Type::getInt1Ty (context)))));
	mu::core::errors::error_list errors;
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::basic_block::insert);
	ctx.push (block);
	ctx.push (instruction);
	auto valid (ctx ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (block->block->getInstList ().size (), 1);
}