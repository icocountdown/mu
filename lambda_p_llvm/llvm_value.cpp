//
//  llvm_value.cpp
//  lambda_p_llvm
//
//  Created by Colin LeMahieu on 9/18/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include <lambda_p_llvm/llvm_value.h>

lambda_p_llvm::llvm_value::llvm_value (::llvm::Value * value_a)
: value (value_a)
{
}

void lambda_p_llvm::llvm_value::operator () ()
{
}