#include <mu/io/analyzer/extensions/extensions.h>

mu::io::analyzer::extensions::extensions::extensions ()
{
}

mu::io::analyzer::extensions::extensions::~extensions ()
{
}

mu::io::analyzer::extensions::extensions::extensions (std::map <mu::string, mu::io::analyzer::extensions::extension *, std::less <mu::string>, gc_allocator <std::pair <mu::string, mu::io::analyzer::extensions::extension *>>> extensions_a)
	: extensions_m (extensions_a)
{
}