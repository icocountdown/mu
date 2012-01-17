#include "tests.h"

#include <lambda_p_llvm_test/module/get_package.h>
#include <lambda_p_llvm_test/module/add_package.h>
#include <lambda_p_llvm_test/linker/link_modules.h>

void lambda_p_llvm_test::tests::run ()
{
	{
		lambda_p_llvm_test::module::get_package test;
		test.run ();
	}
	{
		lambda_p_llvm_test::module::add_package test;
		test.run ();
	}
	{
		lambda_p_llvm_test::linker::link_modules test;
		test.run ();
	}
}