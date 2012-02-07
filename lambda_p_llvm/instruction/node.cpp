#include "node.h"

#include <llvm/Instruction.h>

lambda_p_llvm::instruction::node::node (llvm::Instruction * instruction_a, boost::shared_ptr <lambda_p_llvm::type::node> type_a)
	: lambda_p_llvm::value::node (instruction_a, type_a)
{
}

llvm::Instruction * lambda_p_llvm::instruction::node::instruction ()
{
	auto result (llvm::cast <llvm::Instruction> (value_m));
	return result;
}