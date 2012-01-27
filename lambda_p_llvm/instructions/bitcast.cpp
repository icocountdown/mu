#include "bitcast.h"

#include <lambda_p/errors/error_target.h>
#include <lambda_p_llvm/value/node.h>
#include <lambda_p_llvm/type/node.h>

#include <llvm/Value.h>
#include <llvm/DerivedTypes.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instructions.h>

#include <sstream>

#include <boost/make_shared.hpp>

void lambda_p_llvm::instructions::bitcast::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, llvm::BasicBlock * & context_a, lambda_p::segment <boost::shared_ptr <lambda_p::node>> parameters_a, std::vector <boost::shared_ptr <lambda_p::node>> & results_a)
{
	if (check_size (errors_a, 2, parameters_a.size ()))
	{
		auto one (boost::dynamic_pointer_cast <lambda_p_llvm::value::node> (parameters_a [0]));
		auto two (boost::dynamic_pointer_cast <lambda_p_llvm::type::node> (parameters_a [1]));
		if (one.get () != nullptr)
		{
			if (two.get () != nullptr)
			{
				auto instruction (new llvm::BitCastInst (one->value (), two->type ()));
				context_a->getInstList ().push_back (instruction);
				results_a.push_back (boost::make_shared <lambda_p_llvm::value::node> (instruction));
			}
			else
			{
				invalid_type (errors_a, 1);
			}
		}
		else
		{
			invalid_type (errors_a, 0);
		}
	}
}