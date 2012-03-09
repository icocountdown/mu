#include "extract.h"

#include <mu/llvm_/basic_block/node.h>
#include <mu/llvm_/set_type/node.h>
#include <mu/llvm_/value/node.h>
#include <mu/llvm_/instruction/node.h>
#include <mu/script/check.h>

#include <boost/make_shared.hpp>

#include <llvm/Instructions.h>

void mu::llvm_::set_type::extract::operator () (mu::script::context & context_a)
{
	if (mu::script::check <mu::llvm_::basic_block::node, mu::llvm_::value::node> () (context_a))
	{
		auto one (boost::static_pointer_cast <mu::llvm_::basic_block::node> (context_a.parameters [0]));
		auto two (boost::static_pointer_cast <mu::llvm_::value::node> (context_a.parameters [1]));
		auto set_type (boost::dynamic_pointer_cast <mu::llvm_::set_type::node> (two->type));
		if (set_type.get () != nullptr)
		{
			for (size_t i (0), j (set_type->elements.size ()); i != j; ++i)
			{
				auto extract (llvm::ExtractValueInst::Create (two->value (), i));
				context_a.results.push_back (boost::make_shared <mu::llvm_::instruction::node> (extract, set_type->elements [i]));
			}
		}
		else
		{
			context_a.results.push_back (two);
		}
	}
}

size_t mu::llvm_::set_type::extract::count ()
{
	return 2;
}