#pragma once

#include <mu/core/types.hpp>

#include <llvm/LLVMContext.h>
#include <llvm/DIBuilder.h>
#include <llvm/DebugInfo.h>

#include <boost/dynamic_bitset.hpp>

namespace llvm
{
    class Module;
    class Function;
    class Type;
    class BasicBlock;
    class Value;
    class PHINode;
    class Instruction;
    class ConstantInt;
    class TerminatorInst;
	class DIBuilder;
}
namespace mu
{
    namespace core
    {
        class error;
    }
    namespace llvmc
    {
        namespace skeleton
        {
            class module;
            class function;
            class type;
            class value;
            class branch;
            class node;
            class switch_element;
            class constant_integer;
            enum class function_return_type;
        }
        class generator_result
        {
        public:
            mu::map <mu::string, llvm::Function *> names;
            llvm::Module * module;
        };
        class generator
        {
        public:
            mu::llvmc::generator_result generate (llvm::LLVMContext & context_a, mu::llvmc::skeleton::module * module_a, mu::string const & name_a, mu::string const & path_a);
        };
        class generate_module
        {
        public:
            generate_module (mu::llvmc::skeleton::module * module_a, mu::llvmc::generator_result & target_a, mu::string const & name_a, mu::string const & path_a);
            void generate ();
			llvm::DIBuilder builder;
            mu::map <mu::llvmc::skeleton::function *, llvm::Function *> functions;
            mu::llvmc::skeleton::module * module;
            mu::llvmc::generator_result & target;
			llvm::DIFile file;
        };
        class generate_function;
        class value_data
        {
        public:
            llvm::Value * predicate;
            llvm::Value * value;
        };
        class generate_function
        {
        public:
            generate_function (mu::llvmc::generate_module & module_a, mu::llvmc::skeleton::function * function_a);
            void generate (mu::string const & name_a);
            llvm::Type * generate_type (mu::llvmc::skeleton::type * type_a);
            mu::llvmc::value_data retrieve_value (mu::llvmc::skeleton::value * value_a);
            mu::llvmc::value_data generate_value (mu::llvmc::skeleton::value * value_a);
            mu::llvmc::value_data generate_single (mu::llvmc::skeleton::value * value_a);
            mu::llvmc::value_data generate_branched (mu::llvmc::skeleton::value * value_a);
            void write_selector (llvm::LLVMContext & context, llvm::Value * & selector, uint8_t selector_number);
            llvm::Value * process_predicates (llvm::Value * predicate_a, mu::vector <mu::llvmc::skeleton::node *> const & arguments_a, size_t predicate_position);
            llvm::Value * and_predicates (llvm::Value * left_a, llvm::Value * right_a);
            std::vector <llvm::Value *> generate_result_set ();
            mu::llvmc::generate_module & module;
            llvm::BasicBlock * last;
            llvm::Function * function_m;
            mu::map <mu::llvmc::skeleton::value *, mu::llvmc::value_data> already_generated;
			mu::map <mu::llvmc::skeleton::type *, llvm::DIType> type_information;
            mu::llvmc::skeleton::function * function;
            mu::llvmc::skeleton::function_return_type function_return_type;
            llvm::DISubprogram function_d;
        };
    }
}