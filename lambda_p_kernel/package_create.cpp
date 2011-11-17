#include "package_create.h"

#include <lambda_p/core/statement.h>
#include <lambda_p/core/association.h>
#include <lambda_p/binder/list.h>
#include <lambda_p_kernel/package.h>
#include <lambda_p/errors/error_list.h>

void lambda_p_kernel::package_create::bind (lambda_p::core::statement * statement, lambda_p::binder::list & nodes, lambda_p::errors::error_list & problems)
{
	check_count (1, 0, statement, problems);
	if (problems.errors.empty ())
	{
		nodes [statement->association->declarations [0]] = boost::shared_ptr <lambda_p_kernel::package> (new lambda_p_kernel::package);
	}
}

std::wstring lambda_p_kernel::package_create::binder_name ()
{
	return std::wstring (L"package_create");
}