 #include <gtest/gtest.h>

#include <mu/llvmc/skeleton.hpp>
#include <mu/llvmc/generator.hpp>

#include <llvm/ADT/Triple.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/IPO.h>

static void print_module (llvm::Module & module, std::string & target)
{
    llvm::raw_string_ostream stream (target);
    module.print (stream, nullptr);
}

static llvm::ExecutionEngine * prepare_module_jit (std::unique_ptr <llvm::Module> module_a)
{
    llvm::legacy::PassManager manager;
    manager.add (llvm::createStripSymbolsPass (true));
    manager.run (*module_a);
	std::string error;
    llvm::EngineBuilder builder (std::move (module_a));
	builder.setErrorStr (&error);
    auto engine (builder.create ());
	assert (engine != nullptr);
	engine->finalizeObject ();
    return engine;
}

TEST (llvmc_generator, generate1)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate1", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
	print_module (*result.module, info);
}

extern char const * const generate_empty_expected;

TEST (llvmc_generator, generate_empty)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_empty", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_empty_expected), info);
}

extern char const * const generate_parameter_expected;

TEST (llvmc_generator, generate_parameter)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    function1.name = U"0";
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_parameter", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_parameter_expected), info);
}

TEST (llvmc_generator, generate_pointer_type)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::pointer_type type2 (&type1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type2, U"parameter1");
    function1.parameters.push_back (&parameter1);
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_pointer_type", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
}

extern char const * const generate_parameter_return_expected;

TEST (llvmc_generator, generate_parameter_return)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    function1.returns = {{&type1}};
    function1.results = {{{&parameter1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_parameter_return", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_parameter_return_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
	ASSERT_NE (nullptr, engine);
    auto function3 (engine->getPointerToFunction (function));
	ASSERT_NE (nullptr, function3);
    auto function4 (reinterpret_cast <bool (*) (bool)> (function3));
    auto result_false (function4 (false));
    ASSERT_EQ (false, result_false);
    auto result_true (function4 (true));
    ASSERT_EQ (true, result_true);
}

extern char const * const generate_add_expected;

TEST (llvmc_generator, generate_add)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function1.parameters.push_back (&parameter2);
    mu::llvmc::skeleton::marker add1 (mu::llvmc::instruction_type::add);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&add1, &parameter1, &parameter2}, {});
    ASSERT_EQ (3, instruction1.arguments.size ());
    ASSERT_EQ (0, instruction1.sequenced.size ());
    ASSERT_EQ (&type1, instruction1.type ());
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_add", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_add_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint8_t (*) (uint8_t, uint8_t)> (function2));
    auto result2 (function3 (0, 0));
    ASSERT_EQ (0, result2);
    auto result3 (function3 (0, 1));
    ASSERT_EQ (1, result3);
    auto result4 (function3 (0xff, 1));
    ASSERT_EQ (0, result4);
}

extern char const * const generate_alloca_expected;

TEST (llvmc_generator, generate_alloca)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::alloca);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &type1}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    mu::llvmc::skeleton::pointer_type type2 (&type1);
    function1.returns = {{&type2}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_alloca", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_alloca_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <void * (*) ()> (function2));
    auto result2 (function3 ());
    ASSERT_NE (nullptr, result2);
}

extern char const * const generate_and_expected;

TEST (llvmc_generator, generate_and)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function1.parameters.push_back (&parameter2);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::and_i);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &parameter2}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_and", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_and_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint8_t (*) (uint8_t, uint8_t)> (function2));
    auto result2 (function3 (0, 0));
    ASSERT_EQ (0, result2);
    auto result3 (function3 (0, 0xff));
    ASSERT_EQ (0, result3);
    auto result4 (function3 (0xff, 0x0f));
    ASSERT_EQ (0x0f, result4);
}

extern char const * const generate_ashr_expected;

TEST (llvmc_generator, generate_ashr)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function1.parameters.push_back (&parameter2);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::ashr);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &parameter2}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_ashr", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_ashr_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint8_t (*) (uint8_t, uint8_t)> (function2));
    auto result2 (function3 (0x8f, 3));
    ASSERT_EQ (0xf1, result2);
    auto result3 (function3 (0x0f, 3));
    ASSERT_EQ (0x01, result3);
}

extern char const * const generate_cmpxchg_expected;

TEST (llvmc_generator, DISABLED_generate_cmpxchg)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::pointer_type type2 (&type1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type2, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function1.parameters.push_back (&parameter2);
    mu::llvmc::skeleton::parameter parameter3 (mu::empty_region, function1.entry, &type1, U"parameter3");
    function1.parameters.push_back (&parameter3);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::cmpxchg);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &parameter2, &parameter3}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_cmpxchg", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_cmpxchg_expected), info);
    auto engine (prepare_module_jit (std::move (result.module)));
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint8_t (*) (uint8_t *, uint8_t, uint8_t)> (function2));
    uint8_t val (0xff);
    auto result2 (function3 (&val, 1, 42));
    ASSERT_EQ (0xff, result2);
    auto result3 (function3 (&val, 0xff, 42));
    ASSERT_EQ (42, result3);
}

extern char const * const generate_icmp1_expected;

TEST (llvmc_generator, generate_icmp1)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function1.parameters.push_back (&parameter2);
	mu::llvmc::skeleton::predicate predicate1 (mu::llvmc::predicates::icmp_eq);
    mu::llvmc::skeleton::icmp instruction1 (mu::empty_region, function1.entry, &module.integer_1_type, &predicate1, &parameter1, &parameter2, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_icmp1", U"", 0));
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_icmp1_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <bool (*) (bool, bool)> (function2));
    auto result2 (function3 (false, false));
    ASSERT_EQ (true, result2);
    auto result3 (function3 (false, true));
    ASSERT_EQ (false, result3);
    auto result4 (function3 (true, false));
    ASSERT_EQ (false, result4);
    auto result5 (function3 (true, true));
    ASSERT_EQ (true, result5);
}

extern char const * const generate_load_expected;

TEST (llvmc_generator, generate_load)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::pointer_type type2 (&type1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type2, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::marker add1 (mu::llvmc::instruction_type::load);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&add1, &parameter1}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_load", U"", 0));
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_load_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <bool (*) (bool*)> (function2));
    auto val1 (false);
    function3 (&val1);
    ASSERT_EQ (false, val1);
    auto val2 (true);
    function3 (&val2);
    ASSERT_EQ (true, val2);
}

extern char const * const generate_lshr_expected;

TEST (llvmc_generator, generate_lshr)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function1.parameters.push_back (&parameter2);
    mu::llvmc::skeleton::marker lshr1 (mu::llvmc::instruction_type::lshr);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&lshr1, &parameter1, &parameter2}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_lshr", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_lshr_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint8_t (*) (uint8_t, uint8_t)> (function2));
    auto result2 (function3 (0, 0));
    ASSERT_EQ (0, result2);
    auto result3 (function3 (0xff, 0));
    ASSERT_EQ (0xff, result3);
    auto result4 (function3 (0xff, 4));
    ASSERT_EQ (0x0f, result4);
}

extern char const * const generate_mul_expected;

TEST (llvmc_generator, generate_mul)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function1.parameters.push_back (&parameter2);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::mul);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &parameter2}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_mul", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_mul_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint8_t (*) (uint8_t, uint8_t)> (function2));
    auto result2 (function3 (0, 0));
    ASSERT_EQ (0, result2);
    auto result3 (function3 (0x04, 0x04));
    ASSERT_EQ (0x10, result3);
}

extern char const * const generate_or_expected;

TEST (llvmc_generator, generate_or)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function1.parameters.push_back (&parameter2);
    mu::vector <mu::llvmc::skeleton::node *> arguments1;
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::or_i);
    arguments1.push_back (&marker1);
    arguments1.push_back (&parameter1);
    arguments1.push_back (&parameter2);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &parameter2}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_or", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_or_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint8_t (*) (uint8_t, uint8_t)> (function2));
    auto result2 (function3 (0, 0));
    ASSERT_EQ (0, result2);
    auto result3 (function3 (0x0f, 0));
    ASSERT_EQ (0x0f, result3);
    auto result4 (function3 (0xf0, 0));
    ASSERT_EQ (0xf0, result4);
}

extern char const * const generate_sdiv_expected;

TEST (llvmc_generator, generate_sdiv)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function1.parameters.push_back (&parameter2);
    mu::vector <mu::llvmc::skeleton::node *> arguments1;
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::sdiv);
    arguments1.push_back (&marker1);
    arguments1.push_back (&parameter1);
    arguments1.push_back (&parameter2);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &parameter2}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_sdiv", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_sdiv_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <int8_t (*) (int8_t, int8_t)> (function2));
    auto result2 (function3 (42, 1));
    ASSERT_EQ (42, result2);
    auto result3 (function3 (42, -1));
    ASSERT_EQ (-42, result3);
}

extern char const * const generate_sext_expected;

TEST (llvmc_generator, generate_sext)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::integer_type type2 (16);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::sext);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &type2}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type2}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_sext", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_sext_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <int16_t (*) (int8_t)> (function2));
    auto result2 (function3 (42));
    ASSERT_EQ (42, result2);
    auto result3 (function3 (-42));
    ASSERT_EQ (-42, result3);
}

extern char const * const generate_shl_expected;

TEST (llvmc_generator, generate_shl)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function1.parameters.push_back (&parameter2);
    mu::llvmc::skeleton::marker shl1 (mu::llvmc::instruction_type::shl);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&shl1, &parameter1, &parameter2}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_shl", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_shl_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint8_t (*) (uint8_t, uint8_t)> (function2));
    auto result_false (function3 (0x0f, 4));
    ASSERT_EQ (0xf0, result_false);
}

extern char const * const generate_srem_expected;

TEST (llvmc_generator, generate_srem)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function1.parameters.push_back (&parameter2);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::srem);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &parameter2}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_srem", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_srem_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <int8_t (*) (int8_t, int8_t)> (function2));
    auto result2 (function3 (42, 2));
    ASSERT_EQ (0, result2);
    auto result3 (function3 (42, 5));
    ASSERT_EQ (2, result3);
    auto result4 (function3 (-42, -5));
    ASSERT_EQ (-2, result4);
}

extern char const * const generate_store_expected;

TEST (llvmc_generator, generate_store)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::pointer_type type2 (&type1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type2, U"parameter2");
    function1.parameters.push_back (&parameter2);
    mu::llvmc::skeleton::store instruction1 (mu::empty_region, function1.entry, &module.the_unit_type, &parameter1, &parameter2, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.results = {{{}, {&named1}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_store", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_store_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <void (*) (bool, bool*)> (function2));
    auto val1 (false);
    function3 (true, &val1);
    ASSERT_EQ (true, val1);
    auto val2 (true);
    function3 (false, &val2);
    ASSERT_EQ (false, val2);
}

extern char const * const generate_sub_expected;

TEST (llvmc_generator, generate_sub)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::marker sub1 (mu::llvmc::instruction_type::sub);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&sub1, &parameter1, &parameter1}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_sub", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_sub_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <bool (*) (bool)> (function2));
    auto result_false (function3 (0));
    ASSERT_EQ (0, result_false);
    auto result_true (function3 (1));
    ASSERT_EQ (0, result_true);
}

extern char const * const generate_udiv_expected;

TEST (llvmc_generator, generate_udiv)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function1.parameters.push_back (&parameter2);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::udiv);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &parameter2}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_udiv", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_udiv_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint8_t (*) (uint8_t, uint8_t)> (function2));
    auto result2 (function3 (0x10, 0x10));
    ASSERT_EQ (1, result2);
    auto result3 (function3 (0x80, 0x10));
    ASSERT_EQ (8, result3);
}

extern char const * const generate_urem_expected;

TEST (llvmc_generator, generate_urem)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function1.parameters.push_back (&parameter2);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::urem);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &parameter2}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_urem", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_urem_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint8_t (*) (uint8_t, uint8_t)> (function2));
    auto result2 (function3 (0x80, 0x80));
    ASSERT_EQ (0, result2);
    auto result3 (function3 (0x82, 0x80));
    ASSERT_EQ (2, result3);
}

extern char const * const generate_xor_expected;

TEST (llvmc_generator, generate_xor)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function1.parameters.push_back (&parameter2);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::xor_i);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &parameter2}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_xor", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_xor_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint8_t (*) (uint8_t, uint8_t)> (function2));
    auto result2 (function3 (0xff, 0x00));
    ASSERT_EQ (0xff, result2);
    auto result3 (function3 (0xff, 0xff));
    ASSERT_EQ (0x0, result3);
}

extern char const * const generate_zext_expected;

TEST (llvmc_generator, generate_zext)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::integer_type type2 (16);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::zext);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &type2}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type2}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_zext", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_zext_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint16_t (*) (uint8_t)> (function2));
    auto result2 (function3 (0x0f));
    ASSERT_EQ (0x0f, result2);
    auto result3 (function3 (0xff));
    ASSERT_EQ (0xff, result3);
}

extern char const * const generate_two_return_expected;

TEST (llvmc_generator, generate_two_return)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    function1.returns = {{&type1, &type1}};
    function1.results = {{{&parameter1, &parameter1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_two_return", U"", 0));
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_two_return_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
	struct thing
	{
		bool b0;
		bool b1;
	};
    auto function3 (reinterpret_cast <thing (*) (bool)> (function2));
    /*auto result2 (function3 (false));
    ASSERT_EQ (false, result2.b0);
    ASSERT_EQ (false, result2.b1);
    auto result3 (function3 (true));
    ASSERT_EQ (true, result3.b0);
    ASSERT_EQ (true, result3.b1);*/
}

extern char const * const generate_if_expected;

TEST (llvmc_generator, generate_if)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::vector <mu::llvmc::skeleton::node *> arguments1;
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::switch_i);
    arguments1.push_back (&marker1);
    arguments1.push_back (&parameter1);
    mu::llvmc::skeleton::switch_i instruction1 (function1.entry, {&marker1, &parameter1}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::branch branch1 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer1 (mu::empty_region, &type1, 0);
    mu::llvmc::skeleton::switch_element element1 (mu::empty_region, &branch1, &instruction1, &integer1);
    mu::llvmc::skeleton::named named1 (mu::empty_region, &element1, U"element1");
    mu::llvmc::skeleton::branch branch2 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer2 (mu::empty_region, &type1, 1);
    mu::llvmc::skeleton::switch_element element2 (mu::empty_region, &branch2, &instruction1, &integer2);
    mu::llvmc::skeleton::named named2 (mu::empty_region, &element2, U"element2");
    function1.returns = {{&module.the_unit_type}, {&module.the_unit_type}};
    function1.results = {{{&named1}, {}}, {{&named2}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_if", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_if_expected), info);
}

extern char const * const generate_if_value_expected;

TEST (llvmc_generator, generate_if_value)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::switch_i);
    mu::llvmc::skeleton::switch_i instruction1 (function1.entry, {&marker1, &parameter1}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::branch branch1 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer1 (mu::empty_region, &type1, 0);
    mu::llvmc::skeleton::switch_element element1 (mu::empty_region, &branch1, &instruction1, &integer1);
    mu::llvmc::skeleton::named named1 (mu::empty_region, &element1, U"element1");
    mu::llvmc::skeleton::branch branch2 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer2 (mu::empty_region, &type1, 1);
    mu::llvmc::skeleton::switch_element element2 (mu::empty_region, &branch2, &instruction1, &integer2);
    mu::llvmc::skeleton::named named2 (mu::empty_region, &element2, U"element2");
    mu::llvmc::skeleton::integer_type type2 (32);
    mu::llvmc::skeleton::constant_integer integer3 (mu::empty_region, &type2, 4);
    mu::llvmc::skeleton::constant_integer integer4 (mu::empty_region, &type2, 5);
    function1.returns = {{&module.the_unit_type, &type2}, {&module.the_unit_type, &type2}};
    function1.results = {{{&named1, &integer3}, {}}, {{&named2, &integer4}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_if_value", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_if_value_expected), info);
}

extern char const * const generate_if_join_expected;

TEST (llvmc_generator, generate_if_join)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::switch_i);
    mu::llvmc::skeleton::switch_i instruction1 (function1.entry, {&marker1, &parameter1}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::branch branch1 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer1 (mu::empty_region, &type1, 0);
    mu::llvmc::skeleton::switch_element element1 (mu::empty_region, &branch1, &instruction1, &integer1);
    mu::llvmc::skeleton::named named1 (mu::empty_region, &element1, U"element1");
    mu::llvmc::skeleton::branch branch2 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer2 (mu::empty_region, &type1, 1);
    mu::llvmc::skeleton::switch_element element2 (mu::empty_region, &branch2, &instruction1, &integer2);
    mu::llvmc::skeleton::named named2 (mu::empty_region, &element2, U"element2");
    mu::llvmc::skeleton::join_value join1;
    auto & branch3 (join1.add_branch ());
    branch3.arguments.push_back (&named1);
    auto & branch4 (join1.add_branch ());
    branch4.arguments.push_back (&named2);
    mu::llvmc::skeleton::join_element join2 (mu::empty_region, function1.entry, &join1, named1.type ());
    join1.elements.push_back (&join2);
    mu::llvmc::skeleton::named named3 (mu::empty_region, &join2, U"join1");
    mu::llvmc::skeleton::unit_type unit;
    function1.returns = {{&unit}};
    function1.results = {{{&named3}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_if_join", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_if_join_expected), info);
}

extern char const * const generate_if_join_value_expected;

TEST (llvmc_generator, generate_if_join_value)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::switch_i);
    mu::llvmc::skeleton::switch_i instruction1 (function1.entry, {&marker1, &parameter1}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::branch branch1 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer1 (mu::empty_region, &type1, 0);
    mu::llvmc::skeleton::switch_element element1 (mu::empty_region, &branch1, &instruction1, &integer1);
    mu::llvmc::skeleton::named named1 (mu::empty_region, &element1, U"element1");
    mu::llvmc::skeleton::branch branch2 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer2 (mu::empty_region, &type1, 1);
    mu::llvmc::skeleton::switch_element element2 (mu::empty_region, &branch2, &instruction1, &integer2);
    mu::llvmc::skeleton::named named2 (mu::empty_region, &element2, U"element2");
    mu::llvmc::skeleton::marker marker2 (mu::llvmc::instruction_type::add);
    
    mu::llvmc::skeleton::instruction add1 (mu::empty_region, &branch1, {&marker2, &parameter1, &parameter1}, {&named2});
    mu::llvmc::skeleton::named named3 (mu::empty_region, &add1, U"add1");
    
    mu::llvmc::skeleton::instruction add2 (mu::empty_region, &branch2, {&marker2, &parameter1, &parameter1}, {&named2});
    mu::llvmc::skeleton::named named4 (mu::empty_region, &add2, U"add2");
    
    mu::llvmc::skeleton::join_value join1;
    auto & branch3 (join1.add_branch ());
    branch3.arguments.push_back (&named3);
    auto & branch4 (join1.add_branch ());
    branch4.arguments.push_back (&named4);
    mu::llvmc::skeleton::join_element join2 (mu::empty_region, function1.entry, &join1, named3.type ());
    join1.elements.push_back (&join2);
    mu::llvmc::skeleton::named named5 (mu::empty_region, &join2, U"join1");
    function1.returns = {{&type1}};
    function1.results = {{{&named5}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_if_join_value", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_if_join_value_expected), info);
}

extern char const * const generate_if_join_2value_expected;

TEST (llvmc_generator, generate_if_join_2value)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::vector <mu::llvmc::skeleton::node *> arguments1;
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::switch_i);
    arguments1.push_back (&marker1);
    arguments1.push_back (&parameter1);
    mu::llvmc::skeleton::switch_i instruction1 (function1.entry, {&marker1, &parameter1}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::branch branch1 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer1 (mu::empty_region, &type1, 0);
    mu::llvmc::skeleton::switch_element element1 (mu::empty_region, &branch1, &instruction1, &integer1);
    mu::llvmc::skeleton::named named1 (mu::empty_region, &element1, U"element1");
    mu::llvmc::skeleton::branch branch2 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer2 (mu::empty_region, &type1, 1);
    mu::llvmc::skeleton::switch_element element2 (mu::empty_region, &branch2, &instruction1, &integer2);
    mu::llvmc::skeleton::named named2 (mu::empty_region, &element2, U"element2");
    mu::llvmc::skeleton::marker marker2 (mu::llvmc::instruction_type::add);
    
    mu::llvmc::skeleton::instruction add1 (mu::empty_region, &branch1, {&marker2, &parameter1, &parameter1}, {&named1});
    mu::llvmc::skeleton::named named3 (mu::empty_region, &add1, U"add1");
    
    mu::llvmc::skeleton::instruction add2 (mu::empty_region, &branch2, {&marker2, &parameter1, &parameter1}, {&named2});
    mu::llvmc::skeleton::named named4 (mu::empty_region, &add2, U"add2");
    
    mu::llvmc::skeleton::instruction add3 (mu::empty_region, &branch1, {&marker2, &parameter1, &parameter1}, {&named1});
    mu::llvmc::skeleton::named named5 (mu::empty_region, &add3, U"add3");
    
    mu::llvmc::skeleton::instruction add4 (mu::empty_region, &branch2, {&marker2, &parameter1, &parameter1}, {&named2});
    mu::llvmc::skeleton::named named6 (mu::empty_region, &add4, U"add4");
    
    mu::llvmc::skeleton::join_value join1;
    auto & branch3 (join1.add_branch ());
    branch3.arguments.push_back (&named3);
    auto & branch4 (join1.add_branch ());
    branch4.arguments.push_back (&named4);
    mu::llvmc::skeleton::join_element join3 (mu::empty_region, function1.entry, &join1, named3.type ());
    join1.elements.push_back (&join3);
    mu::llvmc::skeleton::named named7 (mu::empty_region, &join3, U"join1");
    mu::llvmc::skeleton::join_value join2;
    auto & branch5 (join2.add_branch ());
    branch5.arguments.push_back (&named5);
    auto & branch6 (join2.add_branch ());
    branch6.arguments.push_back (&named6);
    mu::llvmc::skeleton::join_element join4 (mu::empty_region, function1.entry, &join2, named5.type ());
    join2.elements.push_back (&join4);
    mu::llvmc::skeleton::named named8 (mu::empty_region, &join4, U"join2");
    function1.returns = {{&type1}, {&type1}};
    function1.results = {{{&named7}, {}}, {{&named8}, {}}};
    
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_if_join_2value", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_if_join_2value_expected), info);
}

extern char const * const generate_if_join_load_expected;

TEST (llvmc_generator, generate_if_join_load)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::pointer_type type2 (&type1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::switch_i);
    mu::llvmc::skeleton::switch_i instruction1 (function1.entry, {&marker1, &parameter1}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::branch branch1 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer1 (mu::empty_region, &type1, 0);
    mu::llvmc::skeleton::switch_element element1 (mu::empty_region, &branch1, &instruction1, &integer1);
    mu::llvmc::skeleton::named named1 (mu::empty_region, &element1, U"element1");
    mu::llvmc::skeleton::branch branch2 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer2 (mu::empty_region, &type1, 1);
    mu::llvmc::skeleton::switch_element element2 (mu::empty_region, &branch2, &instruction1, &integer2);
    mu::llvmc::skeleton::named named2 (mu::empty_region, &element2, U"element2");
    mu::llvmc::skeleton::marker marker2 (mu::llvmc::instruction_type::load);
    
    mu::llvmc::skeleton::constant_pointer_null pointer1 (mu::empty_region, &type2);
    mu::llvmc::skeleton::instruction load1 (mu::empty_region, &branch1, {&marker2, &pointer1}, {&named1});
    mu::llvmc::skeleton::named named3 (mu::empty_region, &load1, U"load1");
    
    mu::llvmc::skeleton::constant_pointer_null pointer2 (mu::empty_region, &type2);
    mu::llvmc::skeleton::instruction load2 (mu::empty_region, &branch2, {&marker2, &pointer2}, {&named2});
    mu::llvmc::skeleton::named named4 (mu::empty_region, &load2, U"load2");
    
    mu::llvmc::skeleton::join_value join1;
    auto & branch3 (join1.add_branch ());
    branch3.arguments.push_back (&named3);
    auto & branch4 (join1.add_branch ());
    branch4.arguments.push_back (&named4);
    mu::llvmc::skeleton::join_element join2 (mu::empty_region, function1.entry, &join1, named3.type ());
    join1.elements.push_back (&join2);
    mu::llvmc::skeleton::named named5 (mu::empty_region, &join2, U"join1");
    function1.returns = {{&type1}};
    function1.results = {{{&named5}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_if_join_load", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_if_join_load_expected), info);
}

extern char const * const generate_call_0_expected;

TEST (llvmc_generator, generate_call_0)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::unit_type type1;
    function1.results = {{}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    
    mu::llvmc::skeleton::function function2 (mu::empty_region);
    mu::vector <mu::llvmc::skeleton::node *> arguments1;
    arguments1.push_back (&function2);
    mu::llvmc::skeleton::function_call call1 (&function1, function2.entry, {&function2}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::call_element element1 (mu::empty_region, function2.entry, &call1, &module.the_unit_type);
    call1.elements.push_back (&element1);
    function2.returns = {{&type1}};
    function2.results = {{{&element1}, {}}};
    function2.name = U"1";
    module.globals.push_back (&function2);
    
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_call_0", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_call_0_expected), info);
}

extern char const * const generate_call_1_expected;

TEST (llvmc_generator, generate_call_1)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    function1.returns = {{&type1}};
    function1.results = {{{&parameter1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    
    mu::llvmc::skeleton::function function2 (mu::empty_region);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function2.parameters.push_back (&parameter2);
    mu::llvmc::skeleton::function_call call1 (&function1, function2.entry, {&function1, &parameter2}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::call_element element1 (mu::empty_region, function2.entry, &call1, &type1);
    call1.elements.push_back (&element1);
    function2.returns = {{&type1}};
    function2.results = {{{&element1}, {}}};
    function2.name = U"1";
    module.globals.push_back (&function2);
    
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_call_1", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_call_1_expected), info);
}

extern char const * const generate_call_2_expected;

TEST (llvmc_generator, generate_call_2)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::switch_i);
    mu::llvmc::skeleton::switch_i instruction1 (function1.entry, {&marker1, &parameter1}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::branch branch1 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer1 (mu::empty_region, &type1, 0);
    mu::llvmc::skeleton::switch_element element1 (mu::empty_region, &branch1, &instruction1, &integer1);
    mu::llvmc::skeleton::branch branch2 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer2 (mu::empty_region, &type1, 1);
    mu::llvmc::skeleton::switch_element element2 (mu::empty_region, &branch2, &instruction1, &integer2);
    function1.returns = {{&module.the_unit_type}, {&module.the_unit_type}};
    function1.results = {{{&element1}, {}}, {{&element2}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    
    mu::llvmc::skeleton::function function2 (mu::empty_region);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function2.parameters.push_back (&parameter2);
    mu::llvmc::skeleton::function_call call1 (&function1, function2.entry, {&function1, &parameter2}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::call_element element3 (mu::empty_region, function2.entry, &call1, &module.the_unit_type);
    call1.elements.push_back (&element3);
    mu::llvmc::skeleton::call_element element4 (mu::empty_region, function2.entry, &call1, &module.the_unit_type);
    call1.elements.push_back (&element4);
    function2.returns = {{element3.type ()}, {element4.type ()}};
    function2.results = {{{&element3}, {}}, {{&element4}, {}}};
    function2.name = U"1";
    module.globals.push_back (&function2);
    
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_call_2", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_call_2_expected), info);
}

extern char const * const generate_call_3_expected;

TEST (llvmc_generator, generate_call_3)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::integer_type type2 (32);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::switch_i);
    mu::llvmc::skeleton::switch_i instruction1 (function1.entry, {&marker1, &parameter1}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::branch branch1 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer1 (mu::empty_region, &type1, 0);
    mu::llvmc::skeleton::switch_element element1 (mu::empty_region, &branch1, &instruction1, &integer1);
    mu::llvmc::skeleton::branch branch2 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer2 (mu::empty_region, &type1, 1);
    mu::llvmc::skeleton::switch_element element2 (mu::empty_region, &branch2, &instruction1, &integer2);
    mu::llvmc::skeleton::constant_integer constant1 (mu::empty_region, &type2, 1);
    mu::llvmc::skeleton::constant_integer constant2 (mu::empty_region, &type2, 2);
    function1.returns = {{&module.the_unit_type, &type2}, {&module.the_unit_type, &type2}};
    function1.results = {{{&element1, &constant1}, {}}, {{&element2, &constant2}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    
    mu::llvmc::skeleton::function function2 (mu::empty_region);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type1, U"parameter2");
    function2.parameters.push_back (&parameter2);
    mu::llvmc::skeleton::function_call call1 (&function1, function2.entry, {&function1, &parameter2}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::call_element element3 (mu::empty_region, function2.entry, &call1, &module.the_unit_type);
    call1.elements.push_back (&element3);
    mu::llvmc::skeleton::call_element element4 (mu::empty_region, function2.entry, &call1, &type2);
    call1.elements.push_back (&element4);
    function2.returns = {{element3.type ()}, {element4.type ()}};
    function2.results = {{{&element3}, {}}, {{&element4}, {}}};
    function2.name = U"1";
    module.globals.push_back (&function2);
    
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_call_3", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_call_3_expected), info);
}

extern char const * const generate_call_predicate_b1v0_expected;

TEST (llvmc_generator, generate_call_predicate_b1v0)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::unit_type type1;
    function1.results = {{{}, {&module.the_unit_value}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    
    mu::llvmc::skeleton::function function2 (mu::empty_region);
    mu::llvmc::skeleton::function_call call1 (&function1, function2.entry, {&function1}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::call_element element1 (mu::empty_region, function2.entry, &call1, &module.the_unit_type);
    call1.elements.push_back (&element1);
    function2.results = {{{}, {&element1}}};
    function2.name = U"1";
    module.globals.push_back (&function2);
    
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_call_predicate_b1v0", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_call_predicate_b1v0_expected), info);
}

extern char const * const generate_call_predicate_b1v1_expected;

TEST (llvmc_generator, generate_call_predicate_b1v1)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::unit_type type1;
    function1.results = {{{}, {&module.the_unit_value}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    
    mu::llvmc::skeleton::function function2 (mu::empty_region);
    mu::llvmc::skeleton::function_call call1 (&function1, function2.entry, {&function1}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::call_element element1 (mu::empty_region, function2.entry, &call1, &module.the_unit_type);
    call1.elements.push_back (&element1);
    mu::llvmc::skeleton::integer_type type2 (1);
    mu::llvmc::skeleton::constant_integer integer1 (mu::empty_region, &type2, 0);
    function2.returns = {{&type2}};
    function2.results = {{{&integer1}, {&element1}}};
    function2.name = U"1";
    module.globals.push_back (&function2);
    
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_call_predicate_b1v1", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_call_predicate_b1v1_expected), info);
}

extern char const * const generate_loop1_expected;

TEST (llvmc_generator, generate_loop1)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
	mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
	function1.parameters.push_back (&parameter1);
	mu::llvmc::skeleton::loop loop1 (&module.the_unit_type);
	mu::llvmc::skeleton::branch loop_entry_branch (function1.entry);
	loop1.loop_entry_branch = &loop_entry_branch;
	loop1.arguments.push_back (&parameter1);
	mu::llvmc::skeleton::loop_parameter loop_parameter1 (mu::empty_region, loop1.loop_entry_branch, &type1, U"parameter2");
	loop1.parameters.push_back (&loop_parameter1);
	mu::llvmc::skeleton::predicate predicate1 (mu::llvmc::predicates::icmp_eq);
	mu::llvmc::skeleton::constant_integer constant_integer1 (mu::empty_region, &type1, 0);
	mu::llvmc::skeleton::icmp instruction1 (mu::empty_region, loop1.loop_entry_branch, &module.integer_1_type, &predicate1, &loop_parameter1, &constant_integer1, {});
	mu::llvmc::skeleton::marker marker2 (mu::llvmc::instruction_type::if_i);
	mu::llvmc::skeleton::switch_i switch1 (loop1.loop_entry_branch, {&marker2, &instruction1}, {}, &module.the_unit_type);
	mu::llvmc::skeleton::constant_integer constant_integer2 (mu::empty_region, &type1, 0);
	mu::llvmc::skeleton::switch_element element1 (mu::empty_region, loop1.loop_entry_branch, &switch1, &constant_integer2);
	mu::llvmc::skeleton::constant_integer constant_integer3 (mu::empty_region, &type1, 1);
	mu::llvmc::skeleton::switch_element element2 (mu::empty_region, loop1.loop_entry_branch, &switch1, &constant_integer3);
    loop1.results = {{{&loop_parameter1}, {&element1}}, {{}, {&element2}}};
	mu::llvmc::skeleton::loop_element element3 (mu::empty_region, function1.entry, &loop1, element1.type ());
	loop1.elements.push_back (&element3);
    function1.results = {{{}, {&element3}}};
    function1.name = U"0";
	module.globals.push_back (&function1);
    
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_loop1", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_loop1_expected), info);
}

extern char const * const generate_loop_count_expected;

TEST (llvmc_generator, generate_loop_count)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (32);
	mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
	function1.parameters.push_back (&parameter1);
	mu::llvmc::skeleton::loop loop1 (&module.the_unit_type);
	mu::llvmc::skeleton::branch loop_entry_branch (function1.entry);
	loop1.loop_entry_branch = &loop_entry_branch;
	loop1.arguments.push_back (&parameter1);
	mu::llvmc::skeleton::constant_integer constant_integer1 (mu::empty_region, &type1, 0);
	loop1.arguments.push_back (&constant_integer1);
	mu::llvmc::skeleton::loop_parameter loop_parameter1 (mu::empty_region, loop1.loop_entry_branch, &type1, U"parameter2"); // Iteration
	mu::llvmc::skeleton::loop_parameter loop_parameter2 (mu::empty_region, loop1.loop_entry_branch, &type1, U"parameter3"); // Total
	loop1.parameters.push_back (&loop_parameter1);
	loop1.parameters.push_back (&loop_parameter2);	
	mu::llvmc::skeleton::predicate predicate1 (mu::llvmc::predicates::icmp_eq);
	mu::llvmc::skeleton::icmp instruction1 (mu::empty_region, loop1.loop_entry_branch, &module.integer_1_type, &predicate1, &loop_parameter1, &constant_integer1, {});
	mu::llvmc::skeleton::integer_type type2 (1);
	mu::llvmc::skeleton::marker marker2 (mu::llvmc::instruction_type::switch_i);
	mu::llvmc::skeleton::constant_integer constant_integer2 (mu::empty_region, &type2, 0);
	mu::llvmc::skeleton::constant_integer constant_integer3 (mu::empty_region, &type2, 1);
	mu::llvmc::skeleton::switch_i switch1 (loop1.loop_entry_branch, {&marker2, &instruction1, &constant_integer2, &constant_integer3}, {}, &module.the_unit_type);
	mu::llvmc::skeleton::branch branch1 (loop1.loop_entry_branch);
	mu::llvmc::skeleton::branch branch2 (loop1.loop_entry_branch);
	mu::llvmc::skeleton::switch_element element1 (mu::empty_region, &branch1, &switch1, &constant_integer2);
	mu::llvmc::skeleton::switch_element element2 (mu::empty_region, &branch2, &switch1, &constant_integer3);
	mu::llvmc::skeleton::marker marker3 (mu::llvmc::instruction_type::add);
	mu::llvmc::skeleton::constant_integer constant_integer4 (mu::empty_region, &type1, 1);
	mu::llvmc::skeleton::instruction instruction2 (mu::empty_region, loop1.loop_entry_branch, {&marker3, &constant_integer4, &loop_parameter1}, {});
	mu::llvmc::skeleton::instruction instruction3 (mu::empty_region, loop1.loop_entry_branch, {&marker3, &constant_integer4, &loop_parameter2}, {});
    loop1.results = {{{&instruction2, &instruction3}, {&element1}}, {{&loop_parameter2}, {&element2}}};
	mu::llvmc::skeleton::branch branch3 (function1.entry);
	mu::llvmc::skeleton::loop_element element3 (mu::empty_region, &branch3, &loop1, loop_parameter2.type ());
	loop1.elements.push_back (&element3);
    function1.returns = {{&type1}};
    function1.results = {{{&element3}, {}}};
    function1.name = U"0";
	module.globals.push_back (&function1);
    
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_loop_count", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_loop_count_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint32_t (*) (uint32_t)> (function2));
	auto result2 (function3 (0 - 5));
	ASSERT_EQ (5, result2);
	auto result3 (function3 (0 - 42));
	ASSERT_EQ (42, result3);
}

extern char const * const generate_asm_expected;

TEST (llvmc_generator, generate_asm)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
	mu::llvmc::skeleton::unit_type type1;
	mu::llvmc::skeleton::asm_c asm1 (&type1, U"text", U"");
	mu::llvmc::skeleton::inline_asm asm2 (mu::empty_region, function1.entry, {&asm1}, {});
    function1.results = {{{}, {&asm2}}};
    function1.name = U"0";
	module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_asm", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_asm_expected), info);
}

extern char const * const generate_getelementptr_expected;

TEST (llvmc_generator, generate_getelementptr)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
	mu::llvmc::skeleton::pointer_type type2 (&type1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type2, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::getelementptr);
	mu::llvmc::skeleton::constant_integer constant1 (mu::empty_region, &type1, 0);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &constant1}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type2}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_getelementptr", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_getelementptr_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint8_t * (*) (uint8_t *)> (function2));
	uint8_t val (0);
    auto result2 (function3 (&val));
    ASSERT_EQ (&val, result2);
}

extern char const * const generate_identity_expected;

TEST (llvmc_generator, generate_identity)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::identity identity2;
    mu::llvmc::skeleton::identity_call identity1 ({&identity2, &parameter1}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::identity_element element1 (function1.entry, &identity1, parameter1.type ());
    identity1.elements.push_back (&element1);
    mu::llvmc::skeleton::named named1 (mu::empty_region, &element1, U"element1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_identity", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_identity_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint8_t (*) (uint8_t)> (function2));
    auto result2 (function3 (42));
    ASSERT_EQ (42, result2);
}

extern char const * const generate_asm2_expected;

TEST (llvmc_generator, generate_asm2)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
	mu::llvmc::skeleton::integer_type type1 (32);
	mu::llvmc::skeleton::asm_c asm1 (&type1, U"bswap $0", U"=r,r");
	mu::vector <mu::llvmc::skeleton::node *> arguments;
	mu::llvmc::skeleton::constant_integer constant1 (mu::empty_region, &type1, 0xffeeddcc);
	arguments.push_back (&asm1);
	arguments.push_back (&constant1);
	mu::llvmc::skeleton::inline_asm asm2 (mu::empty_region, function1.entry, {&asm1, &constant1}, {});
	mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::add);
	mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &asm2, &asm2}, {});
    function1.results = {{{}, {&instruction1}}};
    function1.name = U"0";
	module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_asm2", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_asm2_expected), info);
}

extern char const * const generate_call_0_predicate_expected;

TEST (llvmc_generator, generate_call_0_predicate)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::unit_type type1;
    function1.results = {{}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    
    mu::llvmc::skeleton::function function2 (mu::empty_region);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function2.entry, &module.integer_1_type, U"p0");
    function2.parameters.push_back (&parameter1);
    
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::switch_i);
    mu::llvmc::skeleton::switch_i instruction1 (function2.entry, {&marker1, &parameter1}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::branch branch1 (function1.entry);
	mu::llvmc::skeleton::integer_type type2 (1);
    mu::llvmc::skeleton::constant_integer integer1 (mu::empty_region, &type2, 0);
    mu::llvmc::skeleton::switch_element element2 (mu::empty_region, &branch1, &instruction1, &integer1);
    mu::llvmc::skeleton::named named1 (mu::empty_region, &element2, U"element1");
    mu::llvmc::skeleton::branch branch2 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer2 (mu::empty_region, &type2, 1);
    mu::llvmc::skeleton::switch_element element3 (mu::empty_region, &branch2, &instruction1, &integer2);
    mu::llvmc::skeleton::named named2 (mu::empty_region, &element3, U"element2");
    
    mu::llvmc::skeleton::function_call call1 (&function1, function2.entry, {&function1}, {&element3}, &module.the_unit_type);
    mu::llvmc::skeleton::call_element element1 (mu::empty_region, function2.entry, &call1, &module.the_unit_type);
    call1.elements.push_back (&element1);
    function2.returns = {{&type1}, {&type1}};
    function2.results = {{{&element1}, {}}, {{&element2}, {}}};
    function2.name = U"1";
    module.globals.push_back (&function2);
    
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_call_0_predicate", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_call_0_predicate_expected), info);
}

extern char const * const generate_array_expected;

TEST (llvmc_generator, generate_array)
{
    mu::llvmc::skeleton::module module;
	mu::llvmc::skeleton::fixed_array_type array1 (&module.integer_8_type, 16);
    mu::vector <mu::llvmc::skeleton::constant *> initializer1;
	mu::llvmc::skeleton::integer_type type1 (8);
    mu::llvmc::skeleton::constant_integer constant2 (mu::empty_region, &type1, 0xcd);
    initializer1.assign (16, &constant2);
	mu::llvmc::skeleton::constant_array constant1 (mu::empty_region, &array1, initializer1);
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    function1.returns = {{&array1}};
    function1.results = {{{&constant1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_array", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_array_expected), info);
}

extern char const * const generate_bitcast_expected;

TEST (llvmc_generator, generate_bitcast)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (32);
    mu::llvmc::skeleton::pointer_type type2 (&type1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type2, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::bitcast);
    mu::llvmc::skeleton::integer_type type3 (8);
    mu::llvmc::skeleton::pointer_type type4 (&type3);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &type4}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type4}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_bitcast", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_bitcast_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint8_t * (*) (uint32_t *)> (function2));
    uint32_t val;
    auto result2 (function3 (&val));
    ASSERT_EQ ((void *)&val, (void *)result2);
}

extern char const * const generate_ptrtoint_expected;

TEST (llvmc_generator, generate_ptrtoint)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (32);
    mu::llvmc::skeleton::pointer_type type2 (&type1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type2, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::ptrtoint);
    mu::llvmc::skeleton::integer_type type3 (64);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &type3}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type3}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_ptrtoint", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_ptrtoint_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint64_t (*) (uint32_t *)> (function2));
    uint32_t val;
    auto result2 (function3 (&val));
    ASSERT_EQ ((uint64_t)&val, result2);
}

extern char const * const generate_ptrfromint_expected;

TEST (llvmc_generator, generate_ptrfromint)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (64);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::inttoptr);
    mu::llvmc::skeleton::integer_type type2 (32);
    mu::llvmc::skeleton::pointer_type type3 (&type2);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &type3}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type3}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_ptrfromint", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_ptrfromint_expected), info);
	llvm::Function * function (result.module->getFunctionList ().begin ());
    auto engine (prepare_module_jit (std::move (result.module)));
    auto function2 (engine->getPointerToFunction (function));
    auto function3 (reinterpret_cast <uint32_t * (*) (uint64_t)> (function2));
    auto result2 (function3 (~0));
    ASSERT_EQ (~0, (uint64_t)result2);
}

extern char const * const generate_global_variable_expected;

TEST (llvmc_generator, generate_global_variable)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
	mu::llvmc::skeleton::integer_type type1 (32);
	mu::llvmc::skeleton::constant_integer constant1 (mu::empty_region, &type1, 42);
    mu::llvmc::skeleton::global_variable global1 (mu::empty_region, &constant1);
    global1.name = U"0";
	module.globals.push_back (&global1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_global_variable", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_global_variable_expected), info);
}

extern char const * const generate_if_join_value_predicate_expected;

TEST (llvmc_generator, generate_if_join_value_predicate)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::switch_i);
    mu::llvmc::skeleton::switch_i instruction1 (function1.entry, {&marker1, &parameter1}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::branch branch1 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer1 (mu::empty_region, &type1, 0);
    mu::llvmc::skeleton::switch_element element1 (mu::empty_region, &branch1, &instruction1, &integer1);
    mu::llvmc::skeleton::named named1 (mu::empty_region, &element1, U"element1");
    mu::llvmc::skeleton::branch branch2 (function1.entry);
    mu::llvmc::skeleton::constant_integer integer2 (mu::empty_region, &type1, 1);
    mu::llvmc::skeleton::switch_element element2 (mu::empty_region, &branch2, &instruction1, &integer2);
    mu::llvmc::skeleton::named named2 (mu::empty_region, &element2, U"element2");
	mu::llvmc::skeleton::integer_type type2 (32);
    mu::llvmc::skeleton::constant_integer constant1 (mu::empty_region, &type2, 42);
    mu::llvmc::skeleton::constant_integer constant2 (mu::empty_region, &type2, 13);
    mu::llvmc::skeleton::join_value join1;
    auto & branch3 (join1.add_branch ());
    branch3.arguments.push_back (&constant1);
    branch3.predicates.push_back (&named1);
    auto & branch4 (join1.add_branch ());
    branch4.arguments.push_back (&constant2);
    branch4.predicates.push_back (&named2);
    mu::llvmc::skeleton::join_element join2 (mu::empty_region, function1.entry, &join1, constant1.type ());
    join1.elements.push_back (&join2);
    mu::llvmc::skeleton::named named5 (mu::empty_region, &join2, U"join1");
    function1.returns = {{join2.type ()}};
    function1.results = {{{&named5}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_if_join_value_predicate", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_if_join_value_predicate_expected), info);
}

extern char const * const generate_struct_type_undefined_expected;

TEST (llvmc_generator, generate_struct_type_undefined)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::struct_type type2;
    type2.elements.push_back (&type1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type2, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::undefined undefined1 (mu::empty_region, &mu::llvmc::skeleton::branch::global, &type2);
    function1.returns = {{&type2}};
    function1.results = {{{&undefined1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_struct_type_undefined", U"", 0));
    std::string info;
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    print_module (*result.module, info);
    ASSERT_EQ (std::string (generate_struct_type_undefined_expected), info);
}

extern char const * const generate_insertvalue_expected;

TEST (llvmc_generator, generate_insertvalue)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
    mu::llvmc::skeleton::struct_type type2;
	type2.elements.push_back (&type1);
    mu::llvmc::skeleton::parameter parameter2 (mu::empty_region, function1.entry, &type2, U"parameter2");
    function1.parameters.push_back (&parameter2);
	mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::insertvalue);
	mu::llvmc::skeleton::integer_type type3 (32);
	mu::llvmc::skeleton::constant_integer constant1 (mu::empty_region, &type3, 0);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter2, &parameter1, &constant1}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type2}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_insertvalue", U"", 0));
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_insertvalue_expected), info);
}

extern char const * const generate_select_expected;

TEST (llvmc_generator, generate_select)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type1, U"parameter1");
    function1.parameters.push_back (&parameter1);
	mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::select);
	mu::llvmc::skeleton::integer_type type2 (32);
	mu::llvmc::skeleton::constant_integer constant1 (mu::empty_region, &type2, 0);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &parameter1, &parameter1}, {&constant1});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_select_expected", U"", 0));
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_select_expected), info);
}

extern char const * const generate_extractvalue_expected;

TEST (llvmc_generator, generate_extractvalue)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::integer_type type1 (1);
    mu::llvmc::skeleton::struct_type type2;
	type2.elements.push_back (&type1);
    mu::llvmc::skeleton::parameter parameter1 (mu::empty_region, function1.entry, &type2, U"parameter1");
    function1.parameters.push_back (&parameter1);
	mu::llvmc::skeleton::marker marker1 (mu::llvmc::instruction_type::extractvalue);
	mu::llvmc::skeleton::integer_type type3 (32);
	mu::llvmc::skeleton::constant_integer constant1 (mu::empty_region, &type3, 0);
    mu::llvmc::skeleton::instruction instruction1 (mu::empty_region, function1.entry, {&marker1, &parameter1, &constant1}, {});
    mu::llvmc::skeleton::named named1 (mu::empty_region, &instruction1, U"instruction1");
    function1.returns = {{&type1}};
    function1.results = {{{&named1}, {}}};
    function1.name = U"0";
    module.globals.push_back (&function1);
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_extractvalue", U"", 0));
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_extractvalue_expected), info);
}

TEST (llvmc_generator, generate_entry)
{
    llvm::LLVMContext context;
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    function1.name = U"0";
    module.globals.push_back (&function1);
	module.entry = &function1;
    mu::llvmc::generator generator;
    auto result (generator.generate (context, &module, U"generate_empty", U"", 0));
	ASSERT_NE (nullptr, result.entry);
	ASSERT_EQ (1, result.module->getFunctionList().size ());
	llvm::Function * function2 (result.module->getFunctionList ().begin ());
	ASSERT_NE (nullptr, function2);
	ASSERT_EQ (result.entry, function2);
}

extern char const * const generate_call_out_of_order_expected;

TEST (llvmc_generator, generate_call_out_of_order)
{
    mu::llvmc::skeleton::module module;
    mu::llvmc::skeleton::function function1 (mu::empty_region);
    mu::llvmc::skeleton::unit_type type1;
    function1.results = {{}};
    function1.name = U"0";
    
    mu::llvmc::skeleton::function function2 (mu::empty_region);
    mu::llvmc::skeleton::function_call call1 (&function1, function2.entry, {&function2}, {}, &module.the_unit_type);
    mu::llvmc::skeleton::call_element element1 (mu::empty_region, function2.entry, &call1, &module.the_unit_type);
    call1.elements.push_back (&element1);
    function2.returns = {{&type1}};
    function2.results = {{{&element1}, {}}};
    function2.name = U"1";
    module.globals.push_back (&function2);
    module.globals.push_back (&function1);
    
    mu::llvmc::generator generator;
    llvm::LLVMContext context;
    auto result (generator.generate (context, &module, U"generate_call_out_of_order", U"", 0));
    ASSERT_NE (nullptr, result.module);
    std::string info;
    print_module (*result.module, info);
	llvm::raw_string_ostream info_stream (info);
    auto broken (llvm::verifyModule (*result.module, &info_stream));
    ASSERT_TRUE (!broken);
    ASSERT_EQ (std::string (generate_call_out_of_order_expected), info);
}