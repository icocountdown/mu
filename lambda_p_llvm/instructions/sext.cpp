#include "sext.h"

#include <lambda_p/errors/error_target.h>
#include <lambda_p_llvm/instruction/node.h>
#include <lambda_p_llvm/type/node.h>

#include <llvm/Value.h>
#include <llvm/DerivedTypes.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instructions.h>

#include <sstream>

#include <boost/make_shared.hpp>

void lambda_p_llvm::instructions::sext::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, lambda_p::segment <boost::shared_ptr <lambda_p::node>> parameters_a, std::vector <boost::shared_ptr <lambda_p::node>> & results_a)
{
	auto one (boost::dynamic_pointer_cast <lambda_p_llvm::value::node> (parameters_a [0]));
	auto two (boost::dynamic_pointer_cast <lambda_p_llvm::type::node> (parameters_a [1]));
	if (one.get () != nullptr)
	{
		if (two.get () != nullptr)
		{
			bool one_int (one->value ()->getType ()->isIntegerTy ());
			bool two_int (two->type ()->isIntegerTy ());
			if (one_int && two_int)
			{
				size_t one_bits (one->value ()->getType ()->getPrimitiveSizeInBits ());
				size_t two_bits (two->type ()->getPrimitiveSizeInBits ());
				if (one_bits == two_bits)
				{
					auto instruction (new llvm::SExtInst (one->value (), two->type ()));
					results_a.push_back (boost::make_shared <lambda_p_llvm::instruction::node> (instruction));
				}
				else
				{
					std::wstringstream message;
					message << L"Bit width of argument two is not less than bit width of argument one: ";
					message << one_bits;
					message << L" ";
					message << two_bits;
					(*errors_a) (message.str ());
				}
			}
			else
			{
				std::wstringstream message;
				message << L"Arguments are not integers: ";
				message << one_int;
				message << L" ";
				message << two_int;
				(*errors_a) (message.str ());
			}
		}
		else
		{
			invalid_type (errors_a, parameters_a [1], 1);
		}
	}
	else
	{
		invalid_type (errors_a, parameters_a [0], 0);
	}
}

size_t lambda_p_llvm::instructions::sext::count ()
{
	return 2;
}