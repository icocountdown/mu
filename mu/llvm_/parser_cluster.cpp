#include <mu/llvm_/parser_cluster.h>

#include <mu/io/tokens_implementor.h>
#include <mu/llvm_/parser_parser.h>
#include <mu/llvm_/parser_routine.h>
#include <mu/llvm_/ast_cluster.h>

#include <gc_cpp.h>

mu::llvm_::parser::cluster::cluster (mu::llvm_::parser::parser & parser_a):
parser (parser_a),
cluster_m (new (GC) mu::llvm_::ast::cluster)
{
}

void mu::llvm_::parser::cluster::operator () (mu::io::tokens::token * token_a)
{
    (*token_a) (this);
}

void mu::llvm_::parser::cluster::operator () (mu::io::tokens::divider * token)
{
    unexpected_token (parser, token);
}

void mu::llvm_::parser::cluster::operator () (mu::io::tokens::identifier * token)
{
    unexpected_token (parser, token);
}

void mu::llvm_::parser::cluster::operator () (mu::io::tokens::left_square * token)
{
    auto state_l (new (GC) mu::llvm_::parser::routine (*this));
    parser.state.push (state_l);
}

void mu::llvm_::parser::cluster::operator () (mu::io::tokens::right_square * token)
{
    unexpected_token (parser, token);
}

void mu::llvm_::parser::cluster::operator () (mu::io::tokens::stream_end * token)
{
    auto failed (map.finalize (parser.errors));
    if (!failed)
    {
        parser.target (cluster_m);
    }
    cluster_m = new (GC) mu::llvm_::ast::cluster;
}

void mu::llvm_::parser::cluster::operator () (mu::io::tokens::value * token)
{
    unexpected_token (parser, token);
}