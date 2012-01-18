#include "builder.h"

#include <boost/bind.hpp>

#include <lambda_p_script/integer/extension.h>
#include <lambda_p_script/string/extension.h>
#include <lambda_p_io/analyzer/extensions/global.h>
#include <lambda_p_script/identity/operation.h>
#include <lambda_p_script/unit/operation.h>
#include <lambda_p_io/analyzer/extensions/global.h>
#include <lambda_p_script/closure/apply.h>
#include <lambda_p_script/closure/create.h>
#include <lambda_p_script/closure/hole.h>
#include <lambda_p_script/bool_c/create.h>
#include <lambda_p_script/bool_c/equal.h>
#include <lambda_p_script/chain/operation.h>
#include <lambda_p_io/analyzer/extensions/lambda.h>
#include <lambda_p_script/integer/equal.h>
#include <lambda_p_script/integer/subtract.h>
#include <lambda_p_script/package/add.h>
#include <lambda_p_script/package/create.h>
#include <lambda_p_script/package/get.h>
#include <lambda_p_script/package/remove.h>
#include <lambda_p_script/times/operation.h>
#include <lambda_p_script/exec/operation.h>
#include <lambda_p_script/astring/extension.h>
#include <lambda_p_script/package/node.h>
#include <lambda_p_script/closure/single.h>
#include <lambda_p_script/package/get_recursive.h>
#include <lambda_p_io/analyzer/extensions/extensions.h>
#include <lambda_p_script/print/operation.h>

lambda_p_script_io::builder::builder ()
	: errors (new lambda_p::errors::error_list),
	synthesizer (boost::bind (&lambda_p_script_io::builder::operator (), this, _1)),
	analyzer (boost::bind (&lambda_p_script_io::synthesizer::operator (), &synthesizer, _1), errors, extensions ()),
	parser (errors, boost::bind (&lambda_p_io::analyzer::analyzer::operator (), &analyzer, _1)),
	lexer (errors, boost::bind (&lambda_p_io::parser::parser::operator (), &parser, _1))
{
}

lambda_p_script_io::builder::builder (boost::shared_ptr <lambda_p_io::analyzer::extensions::extensions> extensions_a)
	: errors (new lambda_p::errors::error_list),
	synthesizer (boost::bind (&lambda_p_script_io::builder::operator (), this, _1)),
	analyzer (boost::bind (&lambda_p_script_io::synthesizer::operator (), &synthesizer, _1), errors, extensions_a),
	parser (errors, boost::bind (&lambda_p_io::analyzer::analyzer::operator (), &analyzer, _1)),
	lexer (errors, boost::bind (&lambda_p_io::parser::parser::operator (), &parser, _1))
{
}

void lambda_p_script_io::builder::operator () (boost::shared_ptr <lambda_p_script::routine> routine_a)
{
	routines.push_back (routine_a);
}

boost::shared_ptr <lambda_p_io::analyzer::extensions::extensions> lambda_p_script_io::builder::extensions ()
{
	auto result (boost::shared_ptr <lambda_p_io::analyzer::extensions::extensions> (new lambda_p_io::analyzer::extensions::extensions));
	auto package_root (boost::shared_ptr <lambda_p_script::package::node> (new lambda_p_script::package::node));
	auto package_script (boost::shared_ptr <lambda_p_script::package::node> (new lambda_p_script::package::node));
	package_root->items.insert (std::map <std::wstring, boost::shared_ptr <lambda_p::node>>::value_type (std::wstring (L"script"), package_script));
	auto package_package (boost::shared_ptr <lambda_p_script::package::node> (new lambda_p_script::package::node));
	package_package->items.insert (std::map <std::wstring, boost::shared_ptr <lambda_p::node>>::value_type (std::wstring (L"add"), boost::shared_ptr <lambda_p::node> (new lambda_p_script::package::add)));
	package_package->items.insert (std::map <std::wstring, boost::shared_ptr <lambda_p::node>>::value_type (std::wstring (L"create"), boost::shared_ptr <lambda_p::node> (new lambda_p_script::package::create)));
	package_package->items.insert (std::map <std::wstring, boost::shared_ptr <lambda_p::node>>::value_type (std::wstring (L"get"), boost::shared_ptr <lambda_p::node> (new lambda_p_script::package::get)));
	package_package->items.insert (std::map <std::wstring, boost::shared_ptr <lambda_p::node>>::value_type (std::wstring (L"remove"), boost::shared_ptr <lambda_p::node> (new lambda_p_script::package::remove)));	
	package_script->items.insert (std::map <std::wstring, boost::shared_ptr <lambda_p::node>>::value_type (std::wstring (L"package"), package_package));
	auto package_integer (boost::shared_ptr <lambda_p_script::package::node> (new lambda_p_script::package::node));
	std::vector <boost::shared_ptr <lambda_p::node>> arguments;
	arguments.push_back (package_root);
	auto environment (boost::shared_ptr <lambda_p_script::closure::single> (new lambda_p_script::closure::single (arguments, boost::shared_ptr <lambda_p_script::operation> (new lambda_p_script::package::get_recursive))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"~"), boost::shared_ptr <lambda_p_io::analyzer::extensions::extension> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (environment)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"=>"), boost::shared_ptr <lambda_p_io::analyzer::extensions::extension> (new lambda_p_io::analyzer::extensions::lambda)));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"#"), boost::shared_ptr <lambda_p_io::analyzer::extensions::extension> (new lambda_p_script::integer::extension)));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"`"), boost::shared_ptr <lambda_p_io::analyzer::extensions::extension> (new lambda_p_script::string::extension)));;
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"`a"), boost::shared_ptr <lambda_p_io::analyzer::extensions::extension> (new lambda_p_script::astring::extension)));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L".id"), boost::shared_ptr <lambda_p_io::analyzer::extensions::extension> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::identity::operation)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L".()"), boost::shared_ptr <lambda_p_io::analyzer::extensions::extension> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::unit::operation)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"script/bool_c/create"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::bool_c::create)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"script/bool_c/equal"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::bool_c::equal)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"script/chain/operation"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::chain::operation)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L".apply"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::closure::apply)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L".create"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::closure::create)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"?"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::closure::hole)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"if"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::closure::hole)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"script/integer/equal"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::integer::equal)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"script/integer/subtract"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::integer::subtract)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L".exec"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p_script::exec::operation> (new lambda_p_script::exec::operation (result))))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"script/print/operation"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::print::operation)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"script/package/add"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::package::add)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"script/package/create"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::package::create)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"script/package/get"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::package::get)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"script/package/remove"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::package::remove)))));
	result->extensions_m.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extensions::extension>>::value_type (std::wstring (L"script/times/operation"), boost::shared_ptr <lambda_p_io::analyzer::extensions::global> (new lambda_p_io::analyzer::extensions::global (boost::shared_ptr <lambda_p::node> (new lambda_p_script::times::operation)))));
	return result;
}
