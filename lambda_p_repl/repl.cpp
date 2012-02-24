#include "repl.h"

#include <iostream>
#include <string>
#include <sstream>

#include <boost/bind.hpp>
#include <core/errors/error_list.h>
#include <core/errors/error.h>
#include <lambda_p_repl/cli_stream.h>
#include <lambda_p_script_io/builder.h>
#include <lambda_p_io/source.h>
#include <lambda_p_script/routine.h>
#include <lambda_p_io/lexer/error.h>
#include <lambda_p_io/parser/error.h>
#include <lambda_p_repl/quit/operation.h>
#include <lambda_p_io/analyzer/extensions/global.h>
#include <lambda_p_llvm/api.h>
#include <lambda_p_io/analyzer/extensions/extensions.h>
#include <lambda_p_script/print/operation.h>
#include <lambda_p_io/tokens/left_square.h>
#include <lambda_p_io/tokens/right_square.h>
#include <lambda_p_script/cluster/node.h>

#include <boost/make_shared.hpp>

lambda_p_repl::repl::repl ()
	: stop_m (false)
{
}

void lambda_p_repl::repl::reset ()
{
	stop_m = false;
}

void lambda_p_repl::repl::run ()
{
	run_loop ();
	std::cout << "Exiting";
}

void lambda_p_repl::repl::run_loop ()
{
	while (!stop_m)
	{
		iteration ();
	}
}

void lambda_p_repl::repl::stop ()
{
	stop_m = true;
}

void lambda_p_repl::repl::iteration ()
{
	std::wcout << L"lp> ";
	boost::shared_ptr <lambda_p_io::lexer::character_stream> stream (new lambda_p_repl::cli_stream (std::wcin));
	lambda_p_script_io::builder builder;
	lambda_p_llvm::api api;
	builder.analyzer.extensions->extensions_m.insert (api.extensions.begin (), api.extensions.end ());
	auto quit (boost::shared_ptr <mu::core::node> (new lambda_p_repl::quit::operation (*this)));
	builder.analyzer.extensions->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L".quit"), boost::shared_ptr <lambda_p_io::analyzer::extensions::extension> (new lambda_p_io::analyzer::extensions::global (quit))));
	lambda_p_io::source source (boost::bind (&lambda_p_io::lexer::lexer::operator(), &builder.lexer, _1));
	builder.parser (new lambda_p_io::tokens::left_square (), mu::core::context ());
	source (stream);
	source (L']');
	source ();
	if (builder.errors->errors.empty ())
	{
		bool stop (false);
		if (builder.clusters.size () == 1)
		{
			auto cluster (builder.clusters [0]);
			if (cluster->routines.size () > 0)
			{
				auto errors (boost::shared_ptr <mu::core::errors::error_list> (new mu::core::errors::error_list));
				std::vector <boost::shared_ptr <mu::core::node>> arguments;
				std::vector <boost::shared_ptr <mu::core::node>> results;
				auto routine (cluster->routines [0]);
				routine->perform (errors, arguments, results);
				if (errors->errors.empty ())
				{
					lambda_p_script::print::operation print;
					std::vector <boost::shared_ptr <mu::core::node>> print_results;
					print.perform (errors, results, print_results);
				}
				else
				{
					print_errors (errors);
				}
			}
			else
			{
				std::wcout << L"Cluster does not have a routine: ";
				std::wcout << cluster->routines.size ();
			}
		}
		else
		{
			std::wcout << L"Input was not one cluster: ";
			std::wcout << builder.clusters.size ();
		}
	}
	else
	{
		print_errors (builder.errors);
	}
}

void lambda_p_repl::repl::print_errors (boost::shared_ptr <mu::core::errors::error_list> errors_a)
{
	for (auto i (errors_a->errors.begin ()), j (errors_a->errors.end ()); i != j; ++i)
	{
		std::wcout << (*i).second.string ();
		std::wcout << L' ';
		(*i).first->string (std::wcout);
		std::wcout << L"\n";
	}

}