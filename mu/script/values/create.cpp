#include "create.h"

#include <mu/script/values/operation.h>

#include <boost/make_shared.hpp>

void mu::script::values::create::perform (boost::shared_ptr <mu::core::errors::error_target> errors_a, mu::core::segment <boost::shared_ptr <mu::core::node>> parameters, std::vector <boost::shared_ptr <mu::core::node>> & results)
{
	std::vector <boost::shared_ptr <mu::core::node>> values;
	for (auto i (parameters.begin ()), j (parameters.end ()); i != j; ++i)
	{
		values.push_back (*i);
	}
	results.push_back (boost::make_shared <mu::script::values::operation> (values));
}