#include <mu/llvm_/cluster_node.h>

#include <mu/llvm_/function_node.h>

mu::llvm_::cluster::node::node (mu::llvm_::module::node * module_a):
module (module_a)
{
}

mu::string mu::llvm_::cluster::node::name ()
{
	return mu::string (U"mu::llvm_::cluster::node");
}