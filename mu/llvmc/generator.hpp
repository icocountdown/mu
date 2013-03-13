#pragma once

#include <mu/core/types.hpp>

#include <llvm/LLVMContext.h>

#include <vector>

namespace llvm
{
    class Module;
    class Function;
    class Type;
    class BasicBlock;
    class Value;
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
        }
        class module_result
        {
        public:
            llvm::Module * module;
            mu::core::error * error;
        };
        class generator
        {
        public:
            module_result generate (llvm::LLVMContext & context_a, mu::llvmc::skeleton::module * module_a);
        };
        class generate_function
        {
        public:
            generate_function (llvm::Module * module_a, mu::llvmc::skeleton::function * function_a);
            mu::core::error * generate ();
            llvm::Module * module;
            mu::llvmc::skeleton::function * function;
        };
    }
}