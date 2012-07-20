#include <mu/core/errors/error_list.h>
#include <mu/llvm_/apint/node.h>
#include <mu/llvm_/context/node.h>
#include <mu/llvm_/integer_type/create.h>
#include <mu/script/context.h>

#include <llvm/LLVMContext.h>
#include <llvm/BasicBlock.h>
#include <llvm/DerivedTypes.h>

#include <vector>

#include <gtest/gtest.h>

#include <gc_cpp.h>

TEST (llvm_test, integer1)
{
	llvm::LLVMContext context;
	auto block (llvm::BasicBlock::Create (context));
	mu::core::errors::errors errors (new (GC) mu::core::errors::error_list);
	mu::script::context ctx;
	ctx.push (new (GC) mu::llvm_::integer_type::create);
	ctx.push (new (GC) mu::llvm_::context::node (&context));
	ctx.push (new (GC) mu::llvm_::apint::node (new llvm::APInt (64, 16)));
	auto valid (ctx ());
	EXPECT_EQ (valid, true);
}