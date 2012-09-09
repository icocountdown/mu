#include <mu/llvm_/basic_block_split_return.h>
#include <mu/script/context.h>
#include <mu/core/errors/error_list.h>
#include <mu/script/identity.h>
#include <mu/llvm_/void_type_node.h>
#include <mu/llvm_/integer_type_node.h>
#include <mu/llvm_/set_type_node.h>
#include <mu/llvm_/constant_node.h>
#include <mu/script/cluster_node.h>
#include <mu/llvm_/basic_block_node.h>
#include <mu/script/runtime_routine.h>

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/Function.h>
#include <llvm/BasicBlock.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>

#include <gtest/gtest.h>

#include <gc_cpp.h>

TEST (llvm_test, DISABLED_split_return1)
{/*
	mu::core::errors::error_list errors;
	mu::script::context context (errors);
	context.push (new (GC) mu::script::synthesizer::operation);
	auto cluster (new (GC) mu::core::cluster);
	auto routine (new (GC) mu::core::routine);
	cluster->routines.push_back (routine);
	auto body (new (GC) mu::core::expression);
	routine->body = body;
	body->dependencies.push_back (new (GC) mu::script::identity);
	body->dependencies.push_back (new (GC) mu::core::parameters);
	context.push (cluster);
	auto valid (context ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (context.working_size (), 1);
	auto cl (dynamic_cast <mu::script::cluster::node *> (context.working (0)));
	EXPECT_EQ (cl->routines.size (), 1);
	auto r (cl->routines [0]);
	llvm::LLVMContext ctx;
	auto module (new llvm::Module (llvm::StringRef (), ctx));
	auto function (llvm::Function::Create (llvm::FunctionType::get (llvm::Type::getVoidTy (ctx), false), llvm::GlobalValue::LinkageTypes::PrivateLinkage));
	module->getFunctionList ().push_back (function);
	auto bl (llvm::BasicBlock::Create (ctx));
	function->getBasicBlockList ().push_back (bl);
	auto block (new (GC) mu::llvm_::basic_block::node (bl));
	context.drop ();
	context.push (new (GC) mu::llvm_::basic_block::split_return (block, r));
	context.push (new (GC) mu::llvm_::constant::node (llvm::ConstantInt::get (llvm::Type::getInt32Ty (ctx), 0, false), new (GC) mu::llvm_::integer_type::node (llvm::Type::getInt32Ty (ctx))));
	auto valid2 (context ());
	EXPECT_EQ (valid2, true);
	EXPECT_EQ (context.working_size (), 1);*/
}