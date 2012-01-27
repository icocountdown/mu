#include "extension.h"

#include <lambda_p/expression.h>
#include <lambda_p/routine.h>
#include <lambda_p_io/builder.h>
#include <lambda_p_io/ast/expression.h>
#include <lambda_p_script_io/builder.h>

#include <boost/bind.hpp>

void lambda_p_llvm_test::extension::run ()
{
	run_1 ();
}

void lambda_p_llvm_test::extension::run_1 ()
{
	lambda_p_script_io::builder builder;
	lambda_p_io::source source (boost::bind (&lambda_p_io::lexer::lexer::operator(), &builder.lexer, _1));
	source (L"[.llvm [# d32 add and ashr cmpxchg load lshr mul or sdiv shl srem store sub trunc udiv urem xor]]");
	assert (builder.errors->errors.empty ());
	assert (builder.routines.size () == 1);
}