#include <mu/script/context.h>
#include <mu/script/synthesizer/operation.h>
#include <mu/core/errors/error_list.h>
#include <mu/core/cluster.h>
#include <mu/script/cluster/node.h>
#include <mu/core/routine.h>
#include <mu/core/expression.h>
#include <mu/core/parameters.h>
#include <mu/script/identity/operation.h>
#include <mu/script/runtime/routine.h>
#include <mu/script/runtime/expression.h>
#include <mu/script/runtime/fixed.h>
#include <mu/script/runtime/reference.h>
#include <mu/script/fail/operation.h>
#include <mu/core/reference.h>

#include <gtest/gtest.h>

#include <gc_cpp.h>

TEST (script_test, synthesizer_operation1)
{
	mu::core::errors::error_list errors;
	mu::script::context context (errors);
	context.push (new (GC) mu::script::synthesizer::operation);
	context.push (new (GC) mu::core::cluster);
	auto valid (context ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (context.working_size (), 1);
	auto cluster (dynamic_cast <mu::script::cluster::node *> (context.working (0)));
	EXPECT_NE (cluster, nullptr);
	EXPECT_EQ (cluster->routines.size (), 0);
}

TEST (script_test, synthesizer_operation2)
{
	mu::core::errors::error_list errors;
	mu::script::context context (errors);
	context.push (new (GC) mu::script::synthesizer::operation);
	auto c (new (GC) mu::core::cluster);
	context.push (c);
	auto routine1 (new (GC) mu::core::routine);
	c->routines.push_back (routine1);
	auto body (new (GC) mu::core::expression);
	routine1->body = body;
	auto identity (new (GC) mu::script::identity::operation);
	body->dependencies.push_back (identity);
	auto valid (context ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (context.working_size (), 1);
	auto cluster (dynamic_cast <mu::script::cluster::node *> (context.working (0)));
	EXPECT_NE (cluster, nullptr);
	EXPECT_EQ (cluster->routines.size (), 1);
	auto r (cluster->routines [0]);
	EXPECT_EQ (r->expressions.size (), 1);
	auto e (r->expressions [0]);
	EXPECT_EQ (e->dependencies.size (), 1);
	auto d (dynamic_cast <mu::script::runtime::fixed *> (e->dependencies [0]));
	EXPECT_NE (d, nullptr);
	EXPECT_EQ (d->node, identity);
	context.drop ();
	context.push (r);
	auto valid2 (context ());
	EXPECT_EQ (valid2, true);
	EXPECT_EQ (context.working_size (), 0);
}

TEST (script_test, synthesizer_operation3)
{
	mu::core::errors::error_list errors;
	mu::script::context context (errors);
	context.push (new (GC) mu::script::synthesizer::operation);
	auto c (new (GC) mu::core::cluster);
	context.push (c);
	auto routine1 (new (GC) mu::core::routine);
	c->routines.push_back (routine1);
	auto body (new (GC) mu::core::expression);
	routine1->body = body;
	auto identity (new (GC) mu::script::identity::operation);
	body->dependencies.push_back (identity);
	body->dependencies.push_back (new (GC) mu::core::parameters);
	auto valid (context ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (context.working_size (), 1);
	auto cluster (dynamic_cast <mu::script::cluster::node *> (context.working (0)));
	EXPECT_NE (cluster, nullptr);
	EXPECT_EQ (cluster->routines.size (), 1);
	auto r (cluster->routines [0]);
	EXPECT_EQ (r->expressions.size (), 1);
	auto e (r->expressions [0]);
	EXPECT_EQ (e->dependencies.size (), 2);
	auto d1 (dynamic_cast <mu::script::runtime::fixed *> (e->dependencies [0]));
	EXPECT_NE (d1, nullptr);
	EXPECT_EQ (d1->node, identity);
	auto d2 (dynamic_cast <mu::script::runtime::reference *> (e->dependencies [1]));
	EXPECT_NE (d2, nullptr);
	//EXPECT_EQ (d2->expression, r->parameters);
	context.drop ();
	context.push (r);
	auto valid2 (context ());
	EXPECT_EQ (valid2, true);
	EXPECT_EQ (context.working_size (), 0);
	context.push (r);
	auto n1 (new (GC) mu::core::node);
	context.push (n1);
	auto valid3 (context ());
	EXPECT_EQ (valid3, true);
	ASSERT_EQ (context.working_size (), 1);
	EXPECT_EQ (context.working (0), n1);
	context.drop ();
	context.push (r);
	context.push (n1);
	auto n2 (new (GC) mu::core::node);
	context.push (n2);
	auto valid4 (context ());
	EXPECT_EQ (valid4, true);
	EXPECT_EQ (context.working_size (), 2);
	EXPECT_EQ (context.working (0), n1);
	EXPECT_EQ (context.working (1), n2);
}

TEST (script_test, synthesizer_operation4)
{
	mu::core::errors::error_list errors;
	mu::script::context context (errors);
	context.push (new (GC) mu::script::synthesizer::operation);
	auto c (new (GC) mu::core::cluster);
	context.push (c);
	auto routine1 (new (GC) mu::core::routine);
	c->routines.push_back (routine1);
	auto body (new (GC) mu::core::expression);
	routine1->body = body;
	auto expression1 (new (GC) mu::core::expression);
	auto expression2 (new (GC) mu::core::expression);
	expression1->dependencies.push_back (expression2);
	expression2->dependencies.push_back (expression1);
	body->dependencies.push_back (expression1);
	auto valid (context ());
	EXPECT_EQ (!valid, true);
}

TEST (script_test, synthesizer_operation5)
{
	mu::core::errors::error_list errors;
	mu::script::context context (errors);
	context.push (new (GC) mu::script::synthesizer::operation);
	auto c (new (GC) mu::core::cluster);
	context.push (c);
	auto routine1 (new (GC) mu::core::routine);
	c->routines.push_back (routine1);
	auto body (new (GC) mu::core::expression);
	routine1->body = body;
	auto fail (new (GC) mu::script::fail::operation);
	body->dependencies.push_back (fail);
	auto valid (context ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (context.working_size (), 1);
	auto cluster (dynamic_cast <mu::script::cluster::node *> (context.working (0)));
	EXPECT_NE (cluster, nullptr);
	EXPECT_EQ (cluster->routines.size (), 1);
	auto r (cluster->routines [0]);
	EXPECT_EQ (r->expressions.size (), 1);
	auto e (r->expressions [0]);
	EXPECT_EQ (e->dependencies.size (), 1);
	auto d1 (dynamic_cast <mu::script::runtime::fixed *> (e->dependencies [0]));
	EXPECT_NE (d1, nullptr);
	EXPECT_EQ (d1->node, fail);
	context.drop ();
	context.push (r);
	auto valid2 (context ());
	EXPECT_EQ (!valid2, true);
}

TEST (script_test, synthesizer_operation6)
{
	mu::core::errors::error_list errors;
	mu::script::context context (errors);
	context.push (new (GC) mu::script::synthesizer::operation);
	auto c (new (GC) mu::core::cluster);
	context.push (c);
	auto routine1 (new (GC) mu::core::routine);
	c->routines.push_back (routine1);
	auto body (new (GC) mu::core::expression);
	routine1->body = body;
	auto expression (new (GC) mu::core::expression);
	body->dependencies.push_back (new (GC) mu::script::identity::operation);
	body->dependencies.push_back (expression);
	expression->dependencies.push_back (new (GC) mu::script::identity::operation);
	auto valid (context ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (context.working_size (), 1);
	auto cluster (dynamic_cast <mu::script::cluster::node *> (context.working (0)));
	EXPECT_NE (cluster, nullptr);
	EXPECT_EQ (cluster->routines.size (), 1);
	auto r (cluster->routines [0]);
	context.drop ();
	context.push (r);
	auto valid2 (context ());
	EXPECT_EQ (valid2, true);
	EXPECT_EQ (context.working_size (), 0);
}

TEST (script_test, synthesizer_operation7)
{
	mu::core::errors::error_list errors;
	mu::script::context context (errors);
	context.push (new (GC) mu::script::synthesizer::operation);
	auto c (new (GC) mu::core::cluster);
	context.push (c);
	auto routine1 (new (GC) mu::core::routine);
	c->routines.push_back (routine1);
	auto body (new (GC) mu::core::expression);
	routine1->body = body;
	auto expression (new (GC) mu::core::expression);
	expression->dependencies.push_back (new (GC) mu::script::identity::operation);
	auto n1 (new (GC) mu::core::node);
	expression->dependencies.push_back (n1);
	body->dependencies.push_back (new (GC) mu::script::identity::operation);
	body->dependencies.push_back (new (GC) mu::core::reference (expression, 0));
	auto valid (context ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (context.working_size (), 1);
	auto cluster (dynamic_cast <mu::script::cluster::node *> (context.working (0)));
	EXPECT_NE (cluster, nullptr);
	EXPECT_EQ (cluster->routines.size (), 1);
	auto r (cluster->routines [0]);
	context.drop ();
	context.push (r);
	auto valid2 (context ());
	EXPECT_EQ (valid2, true);
	EXPECT_EQ (context.working_size (), 1);
	EXPECT_EQ (context.working (0), n1);
}

TEST (script_test, synthesizer_operation8)
{
	mu::core::errors::error_list errors;
	mu::script::context context (errors);
	context.push (new (GC) mu::script::synthesizer::operation);
	auto c (new (GC) mu::core::cluster);
	context.push (c);
	auto routine1 (new (GC) mu::core::routine);
	c->routines.push_back (routine1);
	auto body (new (GC) mu::core::expression);
	routine1->body = body;
	auto expression (new (GC) mu::core::expression);
	expression->dependencies.push_back (new (GC) mu::script::identity::operation);
	auto n1 (new (GC) mu::core::node);
	expression->dependencies.push_back (n1);
	auto expression2 (new (GC) mu::core::expression);
	expression2->dependencies.push_back (new (GC) mu::script::identity::operation);
	expression2->dependencies.push_back (new (GC) mu::core::reference (expression, 0));
	body->dependencies.push_back (new (GC) mu::script::identity::operation);
	body->dependencies.push_back (new (GC) mu::core::reference (expression, 0));
	body->dependencies.push_back (expression2);
	auto valid (context ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (context.working_size (), 1);
	auto cluster (dynamic_cast <mu::script::cluster::node *> (context.working (0)));
	EXPECT_NE (cluster, nullptr);
	EXPECT_EQ (cluster->routines.size (), 1);
	auto r (cluster->routines [0]);
	context.drop ();
	context.push (r);
	auto valid2 (context ());
	EXPECT_EQ (valid2, true);
	EXPECT_EQ (context.working_size (), 2);
	EXPECT_EQ (context.working (0), n1);
	EXPECT_EQ (context.working (1), n1);
}

TEST (script_test, synthesizer_operation9)
{
	mu::core::errors::error_list errors;
	mu::script::context context (errors);
	context.push (new (GC) mu::script::synthesizer::operation);
	auto c (new (GC) mu::core::cluster);
	context.push (c);
	auto routine1 (new (GC) mu::core::routine);
	c->routines.push_back (routine1);
	auto body (new (GC) mu::core::expression);
	routine1->body = body;
	auto expression (new (GC) mu::core::expression);
	expression->dependencies.push_back (new (GC) mu::script::identity::operation);
	auto n1 (new (GC) mu::core::node);
	expression->dependencies.push_back (n1);
	auto expression2 (new (GC) mu::core::expression);
	expression2->dependencies.push_back (new (GC) mu::script::identity::operation);
	expression2->dependencies.push_back (new (GC) mu::core::reference (expression, 0));
	body->dependencies.push_back (new (GC) mu::script::identity::operation);
	body->dependencies.push_back (expression2);
	body->dependencies.push_back (new (GC) mu::core::reference (expression, 0));
	auto valid (context ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (context.working_size (), 1);
	auto cluster (dynamic_cast <mu::script::cluster::node *> (context.working (0)));
	EXPECT_NE (cluster, nullptr);
	EXPECT_EQ (cluster->routines.size (), 1);
	auto r (cluster->routines [0]);
	context.drop ();
	context.push (r);
	auto valid2 (context ());
	EXPECT_EQ (valid2, true);
	EXPECT_EQ (context.working_size (), 2);
	EXPECT_EQ (context.working (0), n1);
	EXPECT_EQ (context.working (1), n1);
}

TEST (script_test, synthesizer_operation10)
{
	mu::core::errors::error_list errors;
	mu::script::context context (errors);
	context.push (new (GC) mu::script::synthesizer::operation);
	auto c (new (GC) mu::core::cluster);
	context.push (c);
	auto routine1 (new (GC) mu::core::routine);
	c->routines.push_back (routine1);
	auto routine2 (new (GC) mu::core::routine);
	c->routines.push_back (routine2);
	auto body (new (GC) mu::core::expression);
	routine1->body = body;
	body->dependencies.push_back (new (GC) mu::script::identity::operation);
	body->dependencies.push_back (routine2);
	auto body2 (new (GC) mu::core::expression);
	routine2->body = body;
	body2->dependencies.push_back (new (GC) mu::script::fail::operation);
	auto valid (context ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (context.working_size (), 1);
	auto cluster (dynamic_cast <mu::script::cluster::node *> (context.working (0)));
	EXPECT_NE (cluster, nullptr);
	EXPECT_EQ (cluster->routines.size (), 2);
	auto r (cluster->routines [0]);
	context.drop ();
	context.push (r);
	auto valid2 (context ());
	EXPECT_EQ (valid2, true);
}