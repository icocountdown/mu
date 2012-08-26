#include <mu/io/ast_expression.h>

#include <mu/io/ast_visitor.h>
#include <mu/io/ast_identifier.h>

#include <gc_cpp.h>

mu::io::ast::expression::expression (mu::io::context context_a, mu::vector <mu::io::ast::node *> values_a):
mu::io::ast::node (context_a),
values (values_a),
full_name (new (GC) mu::io::ast::identifier (mu::io::context (), mu::string ()))
{
}

mu::io::ast::expression::expression (mu::io::context context_a, mu::vector <mu::io::ast::node *> values_a, mu::vector <mu::io::ast::identifier *> individual_names_a):
mu::io::ast::node (context_a),
values (values_a),
individual_names (individual_names_a),
full_name (new (GC) mu::io::ast::identifier (mu::io::context (), mu::string ()))
{
}

mu::io::ast::expression::expression (mu::io::context context_a, mu::vector <mu::io::ast::node *> values_a, mu::vector <mu::io::ast::identifier *> individual_names_a, mu::io::ast::identifier * full_name_a):
mu::io::ast::node (context_a),
values (values_a),
individual_names (individual_names_a),
full_name (full_name_a)
{
}

mu::string mu::io::ast::expression::name ()
{
	return mu::string (U"expression");
}

void mu::io::ast::expression::operator () (mu::io::ast::visitor * visitor_a)
{
	(*visitor_a) (this);
}