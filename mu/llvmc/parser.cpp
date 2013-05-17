#include <mu/llvmc/parser.hpp>

#include <boost/lexical_cast.hpp>

#include <mu/core/error_string.hpp>
#include <mu/io/stream.hpp>
#include <mu/io/tokens.hpp>
#include <mu/llvmc/ast.hpp>
#include <mu/llvmc/partial_ast.hpp>
#include <mu/io/stream_token.hpp>
#include <mu/llvmc/skeleton.hpp>

#include <llvm/Type.h>

#include <gc_cpp.h>

mu::llvmc::global::global (mu::llvmc::keywords * keywords_a) :
keywords (keywords_a)
{
}

mu::llvmc::parser::parser (mu::llvmc::partial_ast & stream_a):
globals (&keywords),
current_mapping (&globals),
stream (stream_a)
{
    auto error1 (keywords.insert (U"function", &function));
    assert (!error1);
    auto error2 (keywords.insert (U"int", &int_type));
    assert (!error2);
    auto error3 (keywords.insert (U"set", &set_hook));
    assert (!error3);
    auto error5 (keywords.insert (U"loop", &loop_hook));
    assert (!error5);
    auto error6 (keywords.insert (U"let", &let_hook));
    assert (!error6);
    auto error7 (globals.insert (U"unit_v", new (GC) mu::llvmc::ast::unit));
    assert (!error7);
    auto error8 (globals.insert (U"if", new (GC) mu::llvmc::ast::value (new (GC) mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::if_i))));
    assert (!error8);
    auto error9 (globals.insert (U"join", new (GC) mu::llvmc::ast::value (new (GC) mu::llvmc::skeleton::join)));
    assert (!error9);
}

mu::llvmc::node_result mu::llvmc::module::parse (mu::string const & data_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::node_result result ({nullptr, nullptr});
    auto module (new (GC) mu::llvmc::ast::module);
    while ((result.node == nullptr) and (result.error == nullptr))
    {
        auto item (parser_a.stream.peek ());
        if (item.ast != nullptr)
        {
            auto function (dynamic_cast <mu::llvmc::ast::function *> (item.ast));
            if (function != nullptr)
            {
                parser_a.stream.consume ();
                module->functions.push_back (function);
            }
            else
            {
                result.error = new (GC) mu::core::error_string (U"Expecting a function", mu::core::error_type::expecting_function);
            }        
        }
        else if (item.token != nullptr)
        {
            auto id (item.token->id ());
            switch (id)
            {
                case mu::io::token_id::end:
                    result.node = module;
                    break;
                default:                    
                    result.error = new (GC) mu::core::error_string (U"Expecting function or end of stream", mu::core::error_type::expecting_function_or_end_of_stream);
                    break;
            }
        }
        else
        {
            result.error = item.error;
        }
    }
    if (result.error == nullptr)
    {
        if (!parser_a.globals.unresolved.empty ())
        {
            result.error = new (GC) mu::core::error_string (U"Unresolved symbols", mu::core::error_type::unresolved_symbols);
            result.node = nullptr;
        }
    }
    return result;
}

bool mu::llvmc::module::covering ()
{
    return false;
}

mu::llvmc::node_result mu::llvmc::function_hook::parse (mu::string const & data_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::function parser_l (data_a, parser_a);
    auto previous_mapping (parser_a.current_mapping);
    parser_a.current_mapping = &parser_l.block;
    parser_l.parse ();
    parser_a.current_mapping = previous_mapping;
    return parser_l.result;
}

mu::llvmc::function::function (mu::string const & data_a, mu::llvmc::parser & parser_a):
block (parser_a.current_mapping),
result ({nullptr, nullptr}),
function_m (new (GC) mu::llvmc::ast::function),
parser (parser_a)
{
    assert (data_a.empty ());
}

mu::llvmc::function::~function ()
{
    parser.current_mapping = block.parent;
}

void mu::llvmc::function::parse ()
{
    parser.stream.consume ();
    parse_name ();
    if (result.error == nullptr)
    {
        parse_parameters ();
        if (result.error == nullptr)
        {
            parse_body ();
            if (result.error == nullptr)
            {
                parse_results ();
                if (result.error == nullptr)
                {
                    result.node = function_m;
                }
            }
        }
    }
}

void mu::llvmc::function::parse_name ()
{
    if (parser.stream.peek ().token != nullptr)
    {
        auto id (parser.stream.peek ().token->id ());
        switch (id)
        {
            case mu::io::token_id::identifier:
            {
                auto name (static_cast <mu::io::identifier *> (parser.stream.peek ().token));
                parser.stream.consume ();
                function_m->name = name->string;
                auto error (parser.globals.insert (name->string, function_m));
                if (error)
                {
                    result.error = new (GC) mu::core::error_string (U"Function name already used", mu::core::error_type::function_name_already_used);
                }
            }
                break;
            default:
                result.error = new (GC) mu::core::error_string (U"Expecting identifier", mu::core::error_type::expecting_identifier);
                break;
        }
    }
    else
    {
        result.error = new (GC) mu::core::error_string (U"Expecting function name", mu::core::error_type::expecting_function_name);
    }
}

void mu::llvmc::function::parse_parameters ()
{
    switch (parser.stream.tokens [0]->id ())
    {
        case mu::io::token_id::left_square:
        {
            parser.stream.tokens.consume (1);
            auto done (false);
            while (result.error == nullptr && !done)
            {
                parse_parameter (done);
            }
        }
            break;
        default:
            result.error = new (GC) mu::core::error_string (U"While parsing parameters, expecting left square", mu::core::error_type::parsing_parameters_expecting_left_square);
            break;
    }
}

void mu::llvmc::function::parse_parameter (bool & done_a)
{
    auto node (parser.stream.peek ());
    if (node.ast != nullptr)
    {
        auto type (node.ast);
        parser.stream.consume ();
        if (parser.stream.peek ().token != nullptr)
        {
            auto next_token (parser.stream.peek ().token);
            auto next_id (next_token->id ());
            switch (next_id)
            {
                case mu::io::token_id::identifier:
                {
                    parser.stream.consume ();
                    auto identifier (static_cast <mu::io::identifier *> (next_token));
                    auto argument (new (GC) mu::llvmc::ast::parameter (type));
                    function_m->parameters.push_back (argument);
                    if (block.insert (identifier->string, argument))
                    {
                        result.error = new (GC) mu::core::error_string (U"Unable to use identifier", mu::core::error_type::unable_to_use_identifier);
                    }
                }
                    break;
                default:
                    result.error = new (GC) mu::core::error_string (U"While parsing parameters, expecting an identifier", mu::core::error_type::parsing_parameters_expecting_identifier);
                    break;
            }
        }
        else
        {
            result.error = new (GC) mu::core::error_string (U"Expecting a parameter name", mu::core::error_type::expecting_a_parameter_name);
        }
    }
    else if (node.token != nullptr)
    {
        auto id (parser.stream.peek ().token->id ());
        switch (id)
        {
            case mu::io::token_id::right_square:
                parser.stream.consume ();
                done_a = true;
                break;
            default:
                result.error = new (GC) mu::core::error_string (U"Expecting type or right square", mu::core::error_type::expecting_type_or_right_square);
                break;                
        }
    }
    else
    {
        result.error = node.error;
    }
}

void mu::llvmc::function::parse_body ()
{
    switch (parser.stream.tokens [0]->id ())
    {
        case mu::io::token_id::left_square:
        {
            parser.stream.tokens.consume (1);
            auto done (false);
            while (!done && result.error == nullptr)
            {
                switch (parser.stream.tokens [0]->id ())
                {
                    case mu::io::token_id::right_square:
                        parser.stream.tokens.consume (1);
                        done = true;
                        break;
                    default:
                    {
                        auto next (parser.stream.peek ());
                        if (next.ast != nullptr)
                        {
                            parser.stream.consume ();
                            function_m->roots.push_back (next.ast);
                        }
                        else if (next.token != nullptr)
                        {
                            result.error = new (GC) mu::core::error_string (U"Expecting expression", mu::core::error_type::expecting_expression);
                        }
                        else
                        {
                            result.error = next.error;
                        }
                    }
                        break;
                }
            }
        }
            break;
        default:
            result.error = new (GC) mu::core::error_string (U"Expecting left square", mu::core::error_type::expecting_left_square);
            break;
    }
}

void mu::llvmc::function::parse_results ()
{
    switch (parser.stream.tokens [0]->id ())
    {
        case mu::io::token_id::left_square:
        {
            parser.stream.tokens.consume (1);
            auto next (parser.stream.tokens [0]);
            auto done (false);
            while (result.error == nullptr && !done)
            {
                switch (next->id ())
                {
                    case mu::io::token_id::left_square:
                        parser.stream.tokens.consume (1);
                        parse_result_set ();
                        function_m->branch_ends.push_back (function_m->results.size ());
                        next = parser.stream.tokens [0];
                        break;
                    case mu::io::token_id::right_square:
                        done = true;
                        break;
                    default:
                        result.error = new (GC) mu::core::error_string (U"Expecting identifier or right square", mu::core::error_type::expecting_identifier_or_right_square);
                        break;
                }
            }
        }
            break;
        default:
            result.error = new (GC) mu::core::error_string (U"Expecting left square", mu::core::error_type::expecting_left_square);
            break;
    }
}

void mu::llvmc::function::parse_result_set ()
{
    auto node (parser.stream.peek ());
    auto done (false);
    auto predicates (false);
    while (result.error == nullptr && !done)
    {
        if (node.ast != nullptr)
        {
            if (!predicates)
            {
                auto type (node.ast);
                parser.stream.consume ();
                auto next (parser.stream.peek ());
                if (next.token != nullptr)
                {
                    auto next_id (next.token->id ());
                    switch (next_id)
                    {
                        case mu::io::token_id::identifier:
                        {
                            parser.stream.consume ();
                            auto result (new (GC) mu::llvmc::ast::result (type));
                            function_m->results.push_back (result);
                            block.refer (static_cast <mu::io::identifier *> (next.token)->string,
                                         [result]
                                         (mu::llvmc::ast::node * node_a)
                                         {
                                             result->value = node_a;
                                         });
                            node = parser.stream.peek ();
                        }
                            break;
                        default:
                            result.error = new (GC) mu::core::error_string (U"Expecting identifier", mu::core::error_type::expecting_identifier);
                            break;
                    }
                }
                else if (next.ast != nullptr)
                {
                    result.error = new (GC) mu::core::error_string (U"Expecting result reference", mu::core::error_type::expecting_result_reference);
                }
                else
                {
                    result.error = next.error;
                }
            }
            else
            {
                function_m->results.push_back (node.ast);
            }
        }
        else if (node.token != nullptr)
        {
            auto node_id (node.token->id ());
            switch (node_id)
            {
                case mu::io::token_id::right_square:
                    parser.stream.consume ();
                    done = true;
                    if (!predicates)
                    {
                        function_m->predicate_offsets.push_back (function_m->results.size ());
                    }
                    break;
                case mu::io::token_id::terminator:
                {
                    parser.stream.consume ();
                    node = parser.stream.peek ();
                    if (predicates == false)
                    {
                        predicates = true;
                        function_m->predicate_offsets.push_back (function_m->results.size ());
                    }
                    else
                    {
                        result.error = new (GC) mu::core::error_string (U"Already parsing predicates", mu::core::error_type::already_parsing_predicates);
                    }
                    break;
                }
                case mu::io::token_id::identifier:
                {
                    if (predicates)
                    {
                        auto index (function_m->results.size ());
                        function_m->results.push_back (nullptr);
                        auto function_l (function_m);
                        block.refer (static_cast <mu::io::identifier *> (node.token)->string,
                                     [function_l, index]
                                     (mu::llvmc::ast::node * node_a)
                                     {
                                         function_l->results [index] = node_a;
                                     });
                        parser.stream.consume ();
                        node = parser.stream.peek ();
                    }
                    else
                    {
                        result.error = new (GC) mu::core::error_string (U"Expecting result reference", mu::core::error_type::expecting_result_reference);
                    }
                    break;
                }
                default:
                    result.error = new (GC) mu::core::error_string (U"Expecting right_square", mu::core::error_type::expecting_right_square);
                    break;
            }
        }
        else
        {
            result.error = node.error;
        }
    }
}

bool mu::llvmc::function_hook::covering ()
{
    return false;
}

mu::llvmc::node_result mu::llvmc::parser::parse ()
{
    auto result (module.parse (mu::string (), *this));
    return result;
}

bool mu::llvmc::keywords::insert (mu::string const & identifier_a, mu::llvmc::hook * hook_a)
{
    auto result (false);
    auto existing (mappings.lower_bound (identifier_a));
    if (hook_a->covering () && existing != mappings.end ())
    {
        auto size (identifier_a.length ());
        result = existing->first.compare (0, size, identifier_a, 0, size) == 0;
    }
    if (existing != mappings.begin ())
    {
        --existing;
        if (existing->second->covering ())
        {
            auto size (existing->first.length ());
            result = result || existing->first.compare (0, size, identifier_a, 0, size) == 0;
        }
    }
    if (!result)
    {
        mappings.insert (existing, decltype (mappings)::value_type (identifier_a, hook_a));
    }
    return result;
}

mu::llvmc::hook_result mu::llvmc::keywords::get_hook (mu::string const & identifier_a)
{
    hook_result result ({nullptr, mu::string ()});
    auto existing (mappings.lower_bound (identifier_a));
    mu::llvmc::hook * candidate (nullptr);
    mu::string data;
    bool need_covering (false);
    if (existing != mappings.end () && existing->first == identifier_a)
    {
        candidate = existing->second;
    }
    else
    {
        need_covering = true;
        if (existing != mappings.begin ())
        {
            --existing;
            auto data_position (existing->first.length ());
            if (identifier_a.compare (0, data_position, existing->first) == 0)
            {
                data.append (identifier_a.begin () + data_position, identifier_a.end ());
                candidate = existing->second;
            }
        }
    }
    if (candidate != nullptr)        
    {
        if (need_covering)
        {
            if (candidate->covering ())
            {
                result.data.swap (data);
                result.hook = candidate;
            }
        }
        else
        {
            result.hook = candidate;
        }
    }
    return result;
}

bool mu::llvmc::global::reserve (mu::string const & name_a)
{
    auto existing (mappings.find(name_a));
    auto result (existing != mappings.end ());
    if (result)
    {
        auto hook (keywords->get_hook (name_a));
        result = hook.hook != nullptr;
    }
    return result;
}

bool mu::llvmc::global::get (mu::string const & name_a, boost::function <void (mu::llvmc::ast::node *)> action_a)
{
    auto existing (mappings.find (name_a));
    auto result (existing == mappings.end ());
    if (!result)
    {
        action_a (existing->second);
    }
    return result;
}

void mu::llvmc::global::refer (mu::string const & name_a, boost::function <void (mu::llvmc::ast::node *)> action_a)
{
    auto error (get (name_a, action_a));
    if (error)
    {
        unresolved.insert (decltype (unresolved)::value_type (name_a, action_a));
    }
}

mu::llvmc::block::block (mu::llvmc::mapping * parent_a):
parent (parent_a)
{
}

bool mu::llvmc::block::insert (mu::string const & name_a, mu::llvmc::ast::node * node_a)
{
    auto result (parent->reserve (name_a));
    if (!result)
    {
        auto existing (mappings.lower_bound (name_a));
        result = (existing != mappings.end ()) && (existing->first == name_a);
        if (!result)
        {
            if (existing != mappings.begin())
            {
                --existing;
            }
            mappings.insert (existing, decltype (mappings)::value_type (name_a, node_a));
        }
    }
    return result;
}

bool mu::llvmc::block::reserve (mu::string const & name_a)
{
    auto result (parent->reserve (name_a));
    if (!result)
    {
        auto existing (mappings.find (name_a));
        result = existing != mappings.end ();
        mappings [name_a] = nullptr;
    }
    return result;
}

bool mu::llvmc::block::get (mu::string const & name_a, boost::function <void (mu::llvmc::ast::node *)> action_a)
{
    auto existing (mappings.find (name_a));
    auto result (existing != mappings.end ());
    if (result)
    {
        result = parent->get (name_a, action_a);
    }
    else
    {
        action_a (existing->second);
    }
    return result;
}

void mu::llvmc::block::refer (mu::string const & name_a, boost::function <void (mu::llvmc::ast::node *)> action_a)
{
    auto existing (mappings.find (name_a));
    auto result (existing == mappings.end ());
    if (result)
    {
        result = parent->get (name_a, action_a);
        if (result)
        {
            unresolved.insert (decltype (unresolved)::value_type (name_a, action_a));
        }
    }
    else
    {
        action_a (existing->second);
    }
}

void mu::llvmc::block::accept (mu::multimap <mu::string, boost::function <void (mu::llvmc::ast::node *)>> unresolved_a)
{
    unresolved.insert (unresolved_a.begin (), unresolved_a.end ());
}

void mu::llvmc::global::accept (mu::multimap <mu::string, boost::function <void (mu::llvmc::ast::node *)>> unresolved_a)
{
    unresolved.insert (unresolved_a.begin (), unresolved_a.end ());
}

mu::llvmc::node_result mu::llvmc::int_type::parse (mu::string const & data_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::node_result result ({nullptr, nullptr});
    try
    {
        std::string data_l (data_a.begin (), data_a.end ());        
        unsigned int bits (boost::lexical_cast <unsigned int> (data_l));
        if (bits <= 1024)
        {
            result.node = new (GC) mu::llvmc::ast::value (new (GC) mu::llvmc::skeleton::integer_type (bits));
        }
        else
        {
            result.error = new (GC) mu::core::error_string (U"Bit width too wide", mu::core::error_type::bit_width_too_wide);
        }
    }
    catch (boost::bad_lexical_cast)
    {
        result.error = new (GC) mu::core::error_string (U"Unable to convert number to unsigned integer", mu::core::error_type::unable_to_convert_number_to_unsigned_integer);
    }
    return result;
}

bool mu::llvmc::int_type::covering ()
{
    return true;
}

mu::llvmc::expression::expression (mu::llvmc::parser & parser_a):
result ({nullptr, nullptr}),
parser (parser_a)
{
}

mu::llvmc::ast::definite_expression::definite_expression () :
predicate_position (0 - 1)
{
}

void mu::llvmc::expression::parse ()
{
    auto expression_l (new (GC) mu::llvmc::ast::definite_expression);
    switch (parser.stream.tokens [0]->id ())
    {
        case mu::io::token_id::left_square:
        {
            parser.stream.tokens.consume (1);
            auto done (false);
            while (!done && result.error == nullptr)
            {
                auto next (parser.stream.peek ());
                if (next.ast != nullptr)
                {
                    expression_l->arguments.push_back (next.ast);
                    parser.stream.consume ();
                }
                else if (next.token != nullptr)
                {
                    switch (next.token->id ())
                    {
                        case mu::io::token_id::identifier:
                        {
                            parser.stream.consume ();
                            auto & arguments (expression_l->arguments);
                            auto position (expression_l->arguments.size ());
                            expression_l->arguments.push_back (nullptr);
                            parser.current_mapping->refer(static_cast <mu::io::identifier *> (next.token)->string,
                                [&arguments, position]
                                (mu::llvmc::ast::node * node_a)
                                {
                                    arguments [position] = node_a;
                                });
                            break;
                        }
                        case mu::io::token_id::terminator:
                        {
                            auto position_l (expression_l->predicate_position);
                            if (position_l == (0 - 1))
                            {
                                expression_l->predicate_position = expression_l->arguments.size ();
                            }
                            else
                            {
                                result.error = new (GC) mu::core::error_string (U"Already parsing predicates", mu::core::error_type::already_parsing_predicates);
                            }
                            parser.stream.consume ();
                            break;
                        }
                        case mu::io::token_id::right_square:
                            done = true;
                            break;
                        default:
                            result.error = new (GC) mu::core::error_string (U"Expecting argument or right_square", mu::core::error_type::expecting_argument_or_right_square);
                            break;
                    }
                }
                else
                {
                    result.error = next.error;
                }
            }
        }
            break;
        default:
            result.error = new (GC) mu::core::error_string (U"Expecting left square", mu::core::error_type::expecting_left_square);
            break;
    }
    if (result.error == nullptr)
    {
        result.node = expression_l;
    }
}

mu::llvmc::node_result mu::llvmc::set_hook::parse (mu::string const & data_a, mu::llvmc::parser & parser_a)
{
    assert (data_a.empty ());
    mu::llvmc::node_result result ({nullptr, nullptr});
    parser_a.stream.consume ();
    auto name (parser_a.stream.peek ());
    if (name.token != nullptr)
    {
        auto name_id (name.token->id ());
        switch (name_id)
        {
            case mu::io::token_id::identifier:
            {
                parser_a.stream.consume ();
                auto next (parser_a.stream.peek ());
                if (next.ast != nullptr)
                {
                    auto error (parser_a.current_mapping->insert(static_cast <mu::io::identifier *> (name.token)->string, next.ast));
                    if (error)
                    {
                        result.error = new (GC) mu::core::error_string (U"Unable to use name", mu::core::error_type::unable_to_use_identifier);
                    }
                    else
                    {
                        result.node = next.ast;
                    }
                }
                else if (next.ast != nullptr)
                {
                    result.error = new (GC) mu::core::error_string (U"Expecting an expression", mu::core::error_type::expecting_expression);
                }
                else
                {
                    result.error = next.error;
                }
            }
                break;
            default:
                result.error = new (GC) mu::core::error_string (U"Expecting identifier", mu::core::error_type::expecting_identifier);
                break;
        }
    }
    else if (name.ast != nullptr)
    {
        result.error = new (GC) mu::core::error_string (U"Expecting a name", mu::core::error_type::expecting_name);
    }
    else
    {
        result.error = name.error;
    }
    return result;
}

bool mu::llvmc::set_hook::covering ()
{
    return false;
}

mu::llvmc::node_result mu::llvmc::let_hook::parse (mu::string const & data_a, mu::llvmc::parser & parser_a)
{
    assert (data_a.empty ());
    mu::llvmc::node_result result ({nullptr, nullptr});
    parser_a.stream.consume ();
    auto done (false);
    auto next (parser_a.stream.tokens [0]);
    mu::vector <mu::io::identifier *> identifiers;
    while (!done && result.error == nullptr)
    {
        switch (next->id ())
        {
            case mu::io::token_id::identifier:
                identifiers.push_back (static_cast <mu::io::identifier *> (next));
                parser_a.stream.tokens.consume (1);
                next = parser_a.stream.tokens [0];
                break;
            case mu::io::token_id::left_square:
                parser_a.stream.tokens.consume (1);
                done = true;
                break;
            default:
                result.error = new (GC) mu::core::error_string (U"Expecting identifier or left square", mu::core::error_type::expecting_identifier_or_left_square);
                break;
        }
    }
    if (result.error == nullptr)
    {
        auto set (new (GC) mu::llvmc::ast::set_expression);
        auto done (false);
        while (!done && result.error == nullptr)
        {
            auto node (parser_a.stream.peek ());
            if (node.ast != nullptr)
            {
                set->items.push_back (node.ast);
                parser_a.stream.consume ();
            }
            else if (node.token != nullptr)
            {
                switch (node.token->id ())
                {
                    case mu::io::token_id::identifier:
                    {
                        auto position (set->items.size ());
                        set->items.push_back (nullptr);
                        parser_a.current_mapping->refer (static_cast <mu::io::identifier *> (node.token)->string,
                                                      [set, position]
                                                      (mu::llvmc::ast::node * node_a)
                                                      {
                                                          set->items [position] = node_a;
                                                      });
                        parser_a.stream.consume ();
                    }
                        break;
                    case mu::io::token_id::right_square:
                        done = true;
                        break;
                    default:
                        result.error = new (GC) mu::core::error_string (U"Unexpected token in let statement", mu::core::error_type::unknown_token_in_let_statement);
                        break;
                }
            }
            else
            {
                result.error = node.error;
            }
        }
        if (result.error == nullptr)
        {
            result.node = set;
            size_t index (0);
            size_t total (identifiers.size ());
            for (auto i (identifiers.begin ()), j (identifiers.end ()); i != j; ++i, ++index)
            {
                parser_a.current_mapping->insert ((*i)->string, new (GC) mu::llvmc::ast::element (set, index, total));
            }
        }
    }
    return result;
}

bool mu::llvmc::let_hook::covering ()
{
    return false;
}

bool mu::llvmc::global::insert (mu::string const & identifier_a, mu::llvmc::ast::node * node_a)
{
    auto hook (keywords->get_hook (identifier_a));
    auto result (hook.hook != nullptr);
    if (!result)
    {
        auto existing (mappings.lower_bound (identifier_a));
        result = (existing != mappings.end ()) && (existing->first == identifier_a);
        if (!result)
        {
            if (existing != mappings.begin())
            {
                --existing;
            }
            mappings.insert (existing, decltype (mappings)::value_type (identifier_a, node_a));
            for (auto i (unresolved.find (identifier_a)), j (unresolved.end ()); i != j && i->first == identifier_a; ++i)
            {
                i->second (node_a);
            }
            unresolved.erase (identifier_a);
        }
    }
    return result;
}

mu::llvmc::loop::loop (mu::llvmc::parser & parser_a):
loop_m (new (GC) mu::llvmc::ast::loop),
result ({nullptr, nullptr}),
parser (parser_a)
{
}

void mu::llvmc::loop::parse ()
{
    parser.stream.consume ();
    parse_arguments ();
    if (result.error == nullptr)
    {
        parse_binds ();
        if (result.error == nullptr)
        {
            parse_body ();
            if (result.error == nullptr)
            {
                parse_results ();
                if (result.error == nullptr)
                {
                    result.node = loop_m;
                }
            }
        }
    }
}

void mu::llvmc::loop::parse_arguments ()
{
    switch (parser.stream.tokens [0]->id ())
    {
        case mu::io::token_id::left_square:
        {
            parser.stream.tokens.consume (1);
            auto done (false);
            while (!done)
            {
                auto next (parser.stream.peek ());
                if (next.ast != nullptr)
                {
                    parser.stream.consume ();
                    loop_m->arguments.push_back (next.ast);
                }
                else if (next.token != nullptr)
                {
                    switch (next.token->id ())
                    {
                        case mu::io::token_id::right_square:
                            parser.stream.consume ();
                            done = true;
                            break;
                        case mu::io::token_id::identifier:
                        {
                            parser.stream.consume ();
                            auto & arguments_l (loop_m->arguments);
                            auto position (arguments_l.size ());
                            arguments_l.push_back (nullptr);
                            parser.current_mapping->refer (static_cast <mu::io::identifier *> (next.token)->string,
                                                          [&arguments_l, position]
                                                           (mu::llvmc::ast::node * node_a)
                                                          {
                                                              arguments_l [position] = node_a;
                                                          });
                        }
                            break;
                        default:
                            done = true;
                            result.error = new (GC) mu::core::error_string (U"Expecting argument or right square", mu::core::error_type::expecting_argument_or_right_square);
                            break;
                    }
                }
                else
                {
                    result.error = next.error;
                    done = true;
                }
            }
        }
            break;
        default:
            result.error = new (GC) mu::core::error_string (U"Expecting left square", mu::core::error_type::expecting_left_square);
            break;
    }
}

void mu::llvmc::loop::parse_binds ()
{
    switch (parser.stream.tokens [0]->id ())
    {
        case mu::io::token_id::left_square:
        {
            parser.stream.tokens.consume (1);
            auto done (false);
            while (!done)
            {
                auto next (parser.stream.peek ());
                if (next.ast != nullptr)
                {
                    done = true;
                    result.error = new (GC) mu::core::error_string (U"Expecting identifier", mu::core::error_type::expecting_identifier);
                }
                else if (next.token != nullptr)
                {
                    switch (next.token->id ())
                    {
                        case mu::io::token_id::identifier:
                        {
                            auto parameter (new (GC) mu::llvmc::ast::loop_parameter);
                            loop_m->parameters.push_back (parameter);
                            auto error (parser.current_mapping->insert(static_cast <mu::io::identifier *> (next.token)->string, parameter));
                            if (!error)
                            {
                                parser.stream.consume ();
                            }
                            else
                            {
                                done = true;
                                result.error = new (GC) mu::core::error_string (U"Unable to use identifier", mu::core::error_type::unable_to_use_identifier);
                            }
                        }
                            break;
                        case mu::io::token_id::right_square:
                            done = true;
                            parser.stream.consume ();
                            break;
                        default:
                            done = true;
                            result.error = new (GC) mu::core::error_string (U"Expecting identifier", mu::core::error_type::expecting_identifier);
                            break;
                    }
                }
                else
                {
                    done = true;
                    result.error = next.error;
                }
            }
        }
            break;
        default:
            result.error = new (GC) mu::core::error_string (U"Expecting left square", mu::core::error_type::expecting_left_square);
            break;
    }
}

void mu::llvmc::loop::parse_body ()
{
    switch (parser.stream.tokens [0]->id ())
    {
        case mu::io::token_id::left_square:
        {
            parser.stream.tokens.consume (1);
            auto done (false);
            while (!done)
            {
                auto next (parser.stream.peek ());
                if (next.ast != nullptr)
                {
                    loop_m->roots.push_back (next.ast);
                    parser.stream.consume ();
                }
                else if (next.token != nullptr)
                {
                    switch (next.token->id ())
                    {
                        case mu::io::token_id::right_square:
                            done = true;
                            parser.stream.consume ();
                            break;
                        default:
                            done = true;
                            result.error = new (GC) mu::core::error_string (U"Expecting expression or right square", mu::core::error_type::expecting_expression_or_right_square);
                            break;
                    }
                }
                else
                {
                    result.error = next.error;
                    done = true;
                }
            }
        }
            break;
        default:
            result.error = new (GC) mu::core::error_string (U"Expecting loop body", mu::core::error_type::expecting_loop_body);
            break;
    }
}

void mu::llvmc::loop::parse_results ()
{
    switch (parser.stream.tokens [0]->id ())
    {
        case mu::io::token_id::left_square:
        {
            parser.stream.tokens.consume (1);
            auto done (false);
            while (!done && result.error == nullptr)
            {
                switch (parser.stream.tokens [0]->id ())
                {
                    case mu::io::token_id::left_square:
                    {
                        parser.stream.tokens.consume (1);
                        loop_m->results.push_back (decltype (loop_m->results)::value_type ());
                        auto & set (loop_m->results [loop_m->results.size () - 1]);
                        auto set_done (false);
                        while (!set_done && result.error == nullptr)
                        {
                            auto next (parser.stream.peek ());
                            if (next.ast != nullptr)
                            {
                                result.error = new (GC) mu::core::error_string (U"Expecting result identifiers", mu::core::error_type::expecting_identifier);
                            }
                            else if (next.token != nullptr)
                            {
                                switch (next.token->id ())
                                {
                                    case mu::io::token_id::identifier:
                                    {
                                        parser.stream.consume ();
                                        auto position (set.size ());
                                        set.push_back (nullptr);
                                        parser.current_mapping->refer (static_cast <mu::io::identifier *> (next.token)->string,
                                                                      [&set, position]
                                                                       (mu::llvmc::ast::node * node_a)
                                                                      {
                                                                          set [position] = node_a;
                                                                      });
                                    }
                                        break;
                                    case mu::io::token_id::right_square:
                                        parser.stream.consume ();
                                        set_done = true;
                                        break;
                                    default:
                                        result.error = new (GC) mu::core::error_string (U"Expecting identifier", mu::core::error_type::expecting_identifier);
                                        break;
                                }
                            }
                            else
                            {
                                result.error = next.error;
                            }
                        }
                    }
                        break;
                    case mu::io::token_id::right_square:
                        done = true;
                        break;
                    default:
                        result.error = new (GC) mu::core::error_string (U"Expecting result set or right square", mu::core::error_type::expecting_result_set_or_right_square);
                        break;
                }
            }
        }
            break;
        default:
            result.error = new (GC) mu::core::error_string (U"Expecting loop results", mu::core::error_type::expecting_loop_results);
            break;
    }
}

mu::llvmc::node_result mu::llvmc::loop_hook::parse (mu::string const & data_a, mu::llvmc::parser & parser_a)
{
    assert (data_a.empty ());
    mu::llvmc::loop loop (parser_a);
    loop.parse ();
    return loop.result;
}

bool mu::llvmc::loop_hook::covering ()
{
    return false;
}

mu::llvmc::block::~block ()
{
    parent->accept (unresolved);
}