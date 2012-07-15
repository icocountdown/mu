#include <mu/io/lexer/multiline_comment.h>

#include <mu/io/lexer/lexer.h>
#include <mu/io/lexer/error.h>
#include <mu/core/errors/error_target.h>

#include <gc_cpp.h>

mu::io::lexer::multiline_comment::multiline_comment (mu::io::lexer::lexer & lexer_a)
	: have_colon (false),
	lexer (lexer_a)
{
}

void mu::io::lexer::multiline_comment::lex (wchar_t character)
{	
	if (character != L'\uffff')
	{
		if (have_colon)
		{
			switch (character)
			{
			case L')':
				lexer.state.pop ();
				break;
			case L'(':
				have_colon = false;
				lexer.state.push (new (GC) mu::io::lexer::multiline_comment (lexer));
				break;
			case L':':
				// Remain in have_colon state
				break;
			default:
				have_colon = false;
				break;
			}
		}
		else
		{
			switch (character)
			{
			case L':':
				have_colon = true;
				break;
			}
		}
	}
	else
	{
		(*lexer.errors) (L"End of stream inside multiline comment");
		lexer.state.push (new (GC) mu::io::lexer::error);
	}
}