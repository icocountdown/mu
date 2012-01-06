#include "unbindable_statement_test.h"

#include <lambda_p_kernel/nodes/data.h>
#include <lambda_p/core/routine.h>
#include <lambda_p/core/statement.h>
#include <lambda_p/core/association.h>
#include <lambda_p/binder/list.h>
#include <lambda_p_kernel/apply.h>
#include <lambda_p/errors/error_list.h>

#include <boost/shared_ptr.hpp>

void lambda_p_test::unbindable_statement_test::run ()
{
	boost::shared_ptr <lambda_p_kernel::nodes::data> data (new lambda_p_kernel::nodes::data (std::wstring ()));
	boost::shared_ptr <lambda_p::core::routine> routine (new lambda_p::core::routine);
	size_t declaration (routine->add_declaration ());
	routine->surface->declarations.push_back (declaration);
	lambda_p::core::statement * statement (routine->add_statement ());
	statement->target = declaration;
	boost::shared_ptr <lambda_p::binder::list> nodes (new lambda_p::binder::list);
	(*nodes) [declaration] = data;
	lambda_p_kernel::apply apply;
	lambda_p::errors::error_list problems;
	lambda_p::binder::list declarations;
	apply.core (routine, *nodes, problems, declarations);
	assert (problems.errors.size () == 1);
}