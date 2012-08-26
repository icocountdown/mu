#include <mu/script/extensions_node.h>
#include <mu/script/extensions_merge.h>
#include <mu/script/context.h>
#include <mu/script/string_node.h>
#include <mu/core/errors/error_target.h>
#include <mu/io/keywording_extensions.h>

#include <gc_cpp.h>

#include <gtest/gtest.h>

// Empty merge
TEST (script_test, extensions_merge1)
{
    mu::script::extensions::node extensions1;
    mu::script::extensions::node extensions2;
    mu::script::string::node prefix (mu::string (U""));
    mu::script::extensions::merge merge;
    mu::script::context context;
    context.push (&merge);
    context.push (&extensions1);
    context.push (&prefix);
    context.push (&extensions2);
    auto valid (context ());
    ASSERT_TRUE (valid);
    ASSERT_TRUE (!context.errors ());
}

// Merge in to empty
TEST (script_test, extensions_merge2)
{
    mu::script::extensions::node extensions1;
    mu::script::extensions::node extensions2;
    auto item1 (new (GC) mu::core::node);
    (*extensions2.extensions) (mu::string (U"0"), item1);
    mu::script::string::node prefix (mu::string (U""));
    mu::script::extensions::merge merge;
    mu::script::context context;
    context.push (&merge);
    context.push (&extensions1);
    context.push (&prefix);
    context.push (&extensions2);
    auto valid (context ());
    ASSERT_TRUE (valid);
    ASSERT_TRUE (!context.errors ());
    ASSERT_TRUE (extensions1.extensions->extensions_m.size () == 1);
    ASSERT_TRUE (extensions1.extensions->extensions_m.find (mu::string (U"0")) != extensions1.extensions->extensions_m.end ());
}

// Error existing
TEST (script_test, extensions_merge3)
{
    mu::script::extensions::node extensions1;
    mu::script::extensions::node extensions2;
    auto item1 (new (GC) mu::core::node);
    (*extensions1.extensions) (mu::string (U"0"), item1);
    (*extensions2.extensions) (mu::string (U"0"), item1);
    mu::script::string::node prefix (mu::string (U""));
    mu::script::extensions::merge merge;
    mu::script::context context;
    context.push (&merge);
    context.push (&extensions1);
    context.push (&prefix);
    context.push (&extensions2);
    auto valid (context ());
    ASSERT_TRUE (!valid);
    ASSERT_TRUE (context.errors ());
}