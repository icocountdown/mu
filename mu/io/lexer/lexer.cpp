#include <mu/io/lexer/lexer.h>

#include <mu/io/tokens/identifier.h>
#include <mu/io/tokens/divider.h>
#include <mu/io/lexer/state.h>
#include <mu/io/lexer/begin.h>
#include <mu/io/lexer/error.h>
#include <mu/io/lexer/control.h>
#include <mu/io/lexer/identifier.h>
#include <mu/io/lexer/multiline_comment.h>
#include <mu/io/lexer/singleline_comment.h>
#include <mu/io/lexer/complex_identifier.h>
#include <mu/io/lexer/error_target.h>

#include <boost/circular_buffer.hpp>

#include <gc_cpp.h>

mu::io::lexer::lexer::lexer (mu::core::errors::error_target * errors_a, boost::function <void (mu::io::tokens::token *, mu::io::debugging::context)> target_a)
	: errors (new (GC) mu::io::lexer::error_target (*this, errors_a)),
	target (target_a)
{
	reset ();
}

void mu::io::lexer::lexer::operator () (wchar_t character)
{
	auto state_l (state.top ());
	state_l->lex (character);
	++position.character;
	if (character == L'\n')
	{
		++position.row;
		position.column = 1;
	}
	else
	{
		++position.column;
	}
}

void mu::io::lexer::lexer::reset ()
{
	while (!state.empty ())
	{
		state.pop ();
	}
	state.push (new (GC) mu::io::lexer::begin (*this));
}