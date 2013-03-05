#pragma once

#include <mu/core/types.hpp>
#include <mu/llvmc/instruction_type.hpp>

namespace llvm
{
    class Constant;
}
namespace mu
{
    namespace llvmc
    {        
        class analyzer_function;
        namespace ast
        {
            class node;
        }
        namespace wrapper
        {
            class type;
        }
        namespace skeleton
        {
            class node
            {
            public:
                virtual ~node ();
            };
            class type : virtual public mu::llvmc::skeleton::node
            {
            public:
                virtual bool operator == (mu::llvmc::skeleton::type const & other_a) const = 0;
                bool operator != (mu::llvmc::skeleton::type const & other_a) const;
            };
            class branch
            {
            public:
                branch (mu::llvmc::skeleton::branch * parent_a);
                mu::llvmc::skeleton::branch * parent;
            };
            class value : virtual public mu::llvmc::skeleton::node
            {
            public:
                value (mu::llvmc::skeleton::branch * branch_a);
                virtual mu::llvmc::skeleton::type * type () = 0;
                virtual void process_arguments (mu::llvmc::analyzer_function & analyzer_a, mu::llvmc::ast::node * node_a, mu::vector <mu::llvmc::skeleton::node *> & arguments_a);
                mu::llvmc::skeleton::branch * branch;
            };
            class parameter : public mu::llvmc::skeleton::value
            {
            public:
                parameter (mu::llvmc::skeleton::branch * branch_a, mu::llvmc::skeleton::type * type_a);
                mu::llvmc::skeleton::type * type () override;
                mu::llvmc::skeleton::type * type_m;
            };
            class constant : public mu::llvmc::skeleton::value
            {
                mu::llvmc::skeleton::type * type () override;
            };
            class instruction : public mu::llvmc::skeleton::value
            {
            public:
                instruction (mu::llvmc::skeleton::branch * branch_a, mu::llvmc::instruction_type type_a);
                void process_arguments (mu::llvmc::analyzer_function & analyzer_a, mu::llvmc::ast::node * node_a, mu::vector <mu::llvmc::skeleton::node *> & arguments_a) override;
                mu::llvmc::instruction_type type;
            };
            class integer_type : public mu::llvmc::skeleton::type
            {
            public:
                integer_type (size_t bits_a);
                bool operator == (mu::llvmc::skeleton::type const & other_a) const override;
                size_t bits;
            };
            class struct_type : public mu::llvmc::skeleton::type
            {
            public:
                mu::vector <mu::llvmc::skeleton::type *> elements;
            };
            class unit_type : public mu::llvmc::skeleton::type
            {
            public:
                bool operator == (mu::llvmc::skeleton::type const & other_a) const override;
            };
            class result
            {
            public:
                result (mu::llvmc::skeleton::type * type_a, mu::llvmc::skeleton::value * value_a);
                mu::llvmc::skeleton::type * type;
                mu::llvmc::skeleton::value * value;
            };
            class function;
            class function_type : public mu::llvmc::skeleton::type
            {
            public:
                function_type (mu::llvmc::skeleton::function & function_a);
                bool operator == (mu::llvmc::skeleton::type const & other_a) const override;
                mu::llvmc::skeleton::function & function;
            };
            class function : public mu::llvmc::skeleton::value
            {
            public:
                function ();
                mu::llvmc::skeleton::function_type type_m;
                mu::llvmc::skeleton::type * type () override;
                mu::llvmc::skeleton::branch entry;
                mu::vector <mu::llvmc::skeleton::parameter *> parameters;
                mu::vector <mu::vector <mu::llvmc::skeleton::result *>> results;
            };
            class pointer_type : public mu::llvmc::skeleton::type
            {
            public:
                pointer_type (mu::llvmc::skeleton::type * type_a);
                mu::llvmc::skeleton::type * pointed_type;
            };
            class function_call
            {
            public:
                function_call (mu::llvmc::skeleton::function & target_a, mu::llvmc::skeleton::branch * branch_a, mu::vector <mu::llvmc::skeleton::node *> const & arguments_a);
                mu::llvmc::skeleton::function & target;
                mu::llvmc::skeleton::branch * branch;
                mu::vector <mu::llvmc::skeleton::node *> arguments;
            };
            class switch_call
            {
            public:
                switch_call (mu::llvmc::skeleton::value * target_a);
                mu::llvmc::skeleton::value * target;
            };
            class switch_element : public mu::llvmc::skeleton::value
            {
            public:
                switch_element (mu::llvmc::skeleton::branch * branch_a, mu::llvmc::skeleton::switch_call * call_a, mu::llvmc::skeleton::value * case_a);
                mu::llvmc::skeleton::switch_call * call;
                mu::llvmc::skeleton::value * case_m;
            };
            class element : public mu::llvmc::skeleton::value
            {
            public:
                element (mu::llvmc::skeleton::branch * branch_a, mu::llvmc::skeleton::function_call * call_a, size_t branch_index_a, size_t result_index_a);
                mu::llvmc::skeleton::type * type () override;
                mu::llvmc::skeleton::function_call * call;
                size_t branch_index;
                size_t result_index;
            };
            class module
            {
            public:
                mu::vector <mu::llvmc::skeleton::function *> functions;
            };
        }
    }
}