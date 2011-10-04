//
//  cast_inst_binder.cpp
//  lambda_p_llvm
//
//  Created by Colin LeMahieu on 9/24/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include <lambda_p_llvm/cast_inst_binder.h>

#include <lambda_p/core/statement.h>
#include <lambda_p/core/node.h>
#include <lambda_p_llvm/value.h>
#include <lambda_p_llvm/type.h>
#include <lambda_p_llvm/generation_context.h>
#include <lambda_p/core/reference.h>
#include <lambda_p/core/declaration.h>
#include <lambda_p/core/association.h>

#include <llvm/Instructions.h>
#include <llvm/BasicBlock.h>

#include <sstream>

lambda_p_llvm::cast_inst_binder::cast_inst_binder (::lambda_p_llvm::generation_context & context_a)
: context (context_a)
{
}

void lambda_p_llvm::cast_inst_binder::bind (::lambda_p::core::statement * statement, ::std::map < ::lambda_p::core::node *, ::boost::shared_ptr < ::lambda_p::binder::node_instance> > & instances, ::std::vector < ::boost::shared_ptr < ::lambda_p::errors::error> > & problems)
{
	check_count_only_references (1, 2, statement, problems);
	if (problems.empty ())
	{
		::lambda_p::core::reference * value_node (static_cast < ::lambda_p::core::reference *> (statement->association->parameters [0]));
		::lambda_p::core::node * type_node (static_cast < ::lambda_p::core::reference *> (statement->association->parameters [1]));
        ::boost::shared_ptr < ::lambda_p::binder::node_instance> value_instance (instances [value_node]);
        ::boost::shared_ptr < ::lambda_p_llvm::value> value (::boost::dynamic_pointer_cast < ::lambda_p_llvm::value> (value_instance));
        if (value.get () != NULL)
        {
            ::boost::shared_ptr < ::lambda_p::binder::node_instance> type_instance (instances [type_node]);
            ::boost::shared_ptr < ::lambda_p_llvm::type> type (::boost::dynamic_pointer_cast < ::lambda_p_llvm::type> (type_instance));
            if (type.get () != NULL)
            {
                ::llvm::CastInst * cast (::llvm::CastInst::CreatePointerCast (value->value_m, type->type_m));
                context.block->getInstList ().push_back (cast);
                ::boost::shared_ptr < ::lambda_p_llvm::value> value (new ::lambda_p_llvm::value (cast));
				instances [statement->association->results [0]] = value;
            }
            else 
            {
                add_error (::std::wstring (L"cast_inst_binder is expecting argument 2 to be an llvm type"), problems);
            }
        }
        else
        {
			add_error (::std::wstring (L"cast_inst_binder is expecting argument 1 to be an llvm value"), problems);
        }
    }
}

::std::wstring lambda_p_llvm::cast_inst_binder::binder_name ()
{
	return ::std::wstring (L"cast_inst_binder");
}