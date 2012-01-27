#include "store.h"

#include <lambda_p/errors/error_target.h>
#include <lambda_p_llvm/value/node.h>

#include <llvm/Value.h>
#include <llvm/DerivedTypes.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instructions.h>

#include <sstream>

#include <boost/make_shared.hpp>

void lambda_p_llvm::instructions::store::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, llvm::BasicBlock * & context_a, lambda_p::segment <boost::shared_ptr <lambda_p::node>> parameters_a, std::vector <boost::shared_ptr <lambda_p::node>> & results_a)
{
	if (check_size (errors_a, 2, parameters_a.size ()))
	{
		auto one (boost::dynamic_pointer_cast <lambda_p_llvm::value::node> (parameters_a [0]));
		auto two (boost::dynamic_pointer_cast <lambda_p_llvm::value::node> (parameters_a [1]));
		if (one.get () != nullptr)
		{
			if (two.get () != nullptr)
			{
				auto ptr (llvm::dyn_cast <llvm::PointerType> (two->value ()->getType ()));
				if (ptr != nullptr)
				{
					if (ptr->getElementType () == one->value ()->getType ())
					{
						auto instruction (new llvm::StoreInst (one->value (), two->value ()));
						context_a->getInstList ().push_back (instruction);
					}
					else
					{
						std::wstringstream message;
						message << L"Argument two is not a pointer to the type of argument one";
						(*errors_a) (message.str ());
					}
				}
				else
				{
					std::wstringstream message;
					message << L"Argument 2 is not a pointer";
					(*errors_a) (message.str ());
				}
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