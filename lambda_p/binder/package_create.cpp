#include "package_create.h"

#include <lambda_p/core/statement.h>
#include <lambda_p/core/association.h>
#include <lambda_p/binder/routine_instances.h>
#include <lambda_p/binder/package.h>

void lambda_p::binder::package_create::bind (::lambda_p::core::statement * statement, ::lambda_p::binder::routine_instances & instances, ::std::vector < ::boost::shared_ptr < ::lambda_p::errors::error> > & problems)
{
	check_count (1, 0, statement, problems);
	if (problems.empty ())
	{
		instances [statement->association->results [0]] = ::boost::shared_ptr < ::lambda_p::binder::package> (new ::lambda_p::binder::package);
	}
}

::std::wstring lambda_p::binder::package_create::binder_name ()
{
	return ::std::wstring (L"package_create");
}