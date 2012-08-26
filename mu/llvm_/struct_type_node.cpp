#include <mu/llvm_/struct_type_node.h>

#include <mu/llvm_/context_node.h>
#include <mu/llvm_/type_build.h>

#include <llvm/DerivedTypes.h>
#include <llvm/ADT/ArrayRef.h>

mu::llvm_::struct_type::node::node (mu::llvm_::context::node * context_a, llvm::StructType * struct_type_a)
{
	for (auto i (struct_type_a->element_begin ()), j (struct_type_a->element_end ()); i != j; ++i)
	{
		auto type (*i);
		mu::llvm_::type::build build (context_a, type);
		elements.push_back (build.type);
	}
}

mu::llvm_::struct_type::node::node (mu::llvm_::context::node * context_a):
context (context_a)
{
}

mu::llvm_::struct_type::node::node (mu::llvm_::context::node * context_a, std::vector <mu::llvm_::type::node *, gc_allocator <mu::llvm_::type::node *>> elements_a):
context (context_a),
elements (elements_a)
{
}

llvm::Type * mu::llvm_::struct_type::node::type ()
{
	return struct_type ();
}

llvm::StructType * mu::llvm_::struct_type::node::struct_type ()
{
	std::vector <llvm::Type *> elements_l;
	for (auto i (elements.begin ()), j (elements.end ()); i != j; ++i)
	{
		elements_l.push_back ((*i)->type ());
	}
	auto result (llvm::StructType::get (*context->context, elements_l, false));
	return result;
}