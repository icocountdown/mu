#include "create.h"

#include <lambda_p_script/package/node.h>

void lambda_p_script::package::create::operator () (boost::shared_ptr <mu::core::errors::error_target> errors_a, mu::core::segment <boost::shared_ptr <mu::core::node>> parameters, std::vector <boost::shared_ptr <mu::core::node>> & results)
{
	results.push_back (boost::shared_ptr <lambda_p_script::package::node> (new lambda_p_script::package::node));
}

std::wstring lambda_p_script::package::create::name ()
{
	return std::wstring (L"lambda_p_script::package::create");
}

size_t lambda_p_script::package::create::count ()
{
	return 0;
}