#include "routine_binder_test_1.h"

#include <lambda_p/binder/routine_binder.h>
#include <lambda_p/core/routine.h>
#include <lambda_p/binder/null_binder.h>
#include <lambda_p/core/association.h>
#include <lambda_p/core/statement.h>
#include <lambda_p/binder/data.h>

#include <boost/shared_ptr.hpp>

lambda_p_test::routine_binder_test_1::routine_binder_test_1(void)
{
}

lambda_p_test::routine_binder_test_1::~routine_binder_test_1(void)
{
}

void lambda_p_test::routine_binder_test_1::run ()
{
	run_1 ();
	run_2 ();
	run_3 ();
	run_4 ();
	run_5 ();
	run_6 ();
	run_7 ();
}

void lambda_p_test::routine_binder_test_1::run_1 ()
{
	::boost::shared_ptr < ::lambda_p::core::routine> routine (new ::lambda_p::core::routine);
	::lambda_p::binder::routine_binder routine_binder (routine);
	routine_binder ();
	assert (!routine_binder.error ());
}

void lambda_p_test::routine_binder_test_1::run_2 ()
{
	::boost::shared_ptr < ::lambda_p::core::routine> routine (new ::lambda_p::core::routine);
	::lambda_p::binder::routine_binder routine_binder (routine);
    size_t declaration (routine->add_declaration ());
	routine->surface->results.push_back (declaration);
	::lambda_p::core::statement * statement (routine->add_statement (declaration));
	routine_binder ();
	assert (routine_binder.error ());
}

void lambda_p_test::routine_binder_test_1::run_3 ()
{
	::boost::shared_ptr < ::lambda_p::core::routine> routine (new ::lambda_p::core::routine);
	::lambda_p::binder::routine_binder routine_binder (routine);
	size_t declaration (routine->add_declaration ());
	routine->surface->results.push_back (declaration);
	::lambda_p::core::statement * statement (routine->add_statement (declaration));
	routine_binder.routine->instances [declaration] = ::boost::shared_ptr < ::lambda_p::binder::node_binder> (new ::lambda_p::binder::null_binder);
	routine_binder ();
	assert (!routine_binder.error ());
}

void lambda_p_test::routine_binder_test_1::run_4 ()
{
	::boost::shared_ptr < ::lambda_p::core::routine> routine (new ::lambda_p::core::routine);
	size_t declaration (routine->add_declaration ());
	routine->surface->results.push_back (declaration);
    size_t declaration2 (routine->add_declaration ());
	routine->surface->results.push_back (declaration2);
	::lambda_p::core::statement * statement (routine->add_statement (declaration));
	statement->association->parameters.push_back (declaration2);
	::lambda_p::binder::routine_binder routine_binder (routine);
	routine_binder.routine->instances [declaration] = ::boost::shared_ptr < ::lambda_p::binder::node_binder> (new ::lambda_p::binder::null_binder);
	routine_binder ();
	assert (routine_binder.error ());
}

void lambda_p_test::routine_binder_test_1::run_5 ()
{
	::boost::shared_ptr < ::lambda_p::core::routine> routine (new ::lambda_p::core::routine);
	size_t declaration (routine->add_declaration ());
	routine->surface->results.push_back (declaration);
	routine->surface->results.push_back (routine->add_declaration ());
	::lambda_p::core::statement * statement (routine->add_statement (declaration));
	statement->association->parameters.push_back (routine->add_data (::std::wstring ()));
	::lambda_p::binder::routine_binder routine_binder (routine);
	routine_binder.routine->instances [declaration] = ::boost::shared_ptr < ::lambda_p::binder::node_binder> (new ::lambda_p::binder::null_binder);
	routine_binder ();
	assert (!routine_binder.error ());
}

void lambda_p_test::routine_binder_test_1::run_6 ()
{
	::boost::shared_ptr < ::lambda_p::core::routine> routine (new ::lambda_p::core::routine);
	size_t declaration (routine->add_declaration ());
	routine->surface->results.push_back (declaration);
	::lambda_p::core::statement * statement (routine->add_statement (declaration));
	::lambda_p::binder::routine_binder routine_binder (routine);
	routine_binder.routine->instances [declaration] = ::boost::shared_ptr < ::lambda_p::binder::node_binder> (new ::lambda_p::binder::null_binder);
	routine_binder ();
	assert (!routine_binder.error ());
	assert (routine_binder.routine->instances [statement->target].get () != NULL);
}

void lambda_p_test::routine_binder_test_1::run_7 ()
{
	::boost::shared_ptr < ::lambda_p::core::routine> routine (new ::lambda_p::core::routine);
	size_t declaration (routine->add_declaration ());
	routine->surface->results.push_back (declaration);
	::lambda_p::core::statement * statement (routine->add_statement (declaration));
	::lambda_p::core::statement * s2 (routine->add_statement (declaration));
	size_t d2 (routine->add_declaration ());
	s2->association->results.push_back (d2);
	size_t r (d2);
	statement->association->parameters.push_back (r);
	::lambda_p::binder::routine_binder routine_binder (routine);
	routine_binder.routine->instances [declaration] = ::boost::shared_ptr < ::lambda_p::binder::node_binder> (new ::lambda_p::binder::null_binder);
	routine_binder ();
	assert (!routine_binder.error ());
	assert (routine_binder.routine->instances [statement->target].get () != NULL);
	assert (routine_binder.routine->instances [declaration].get () != NULL);
	assert (routine_binder.routine->instances [r].get () != NULL);
}