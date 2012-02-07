#include "operation.h"

#include <lambda_p/errors/error_target.h>
#include <lambda_p_io/ast/cluster.h>
#include <lambda_p_llvm/module/node.h>
#include <lambda_p_llvm/analyzer/operation.h>
#include <lambda_p_script_io/synthesizer.h>
#include <lambda_p/cluster.h>
#include <lambda_p_script/cluster/node.h>
#include <lambda_p_script/routine.h>
#include <lambda_p_llvm/function_type/node.h>
#include <lambda_p_llvm/basic_block/node.h>

#include <llvm/Module.h>

void lambda_p_llvm::synthesizer::operation::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, lambda_p::segment <boost::shared_ptr <lambda_p::node>> parameters, std::vector <boost::shared_ptr <lambda_p::node>> & results)
{
	auto one (boost::dynamic_pointer_cast <lambda_p_io::ast::cluster> (parameters [0]));
	auto two (boost::dynamic_pointer_cast <lambda_p_llvm::module::node> (parameters [1]));
	if (one.get () != nullptr)
	{
		if (two.get () != nullptr)
		{
			lambda_p_llvm::analyzer::operation analyzer;
			std::vector <boost::shared_ptr <lambda_p::node>> arguments;
			std::vector <boost::shared_ptr <lambda_p::node>> results;
			arguments.push_back (one);
			analyzer (errors_a, arguments, results);
			if (!(*errors_a) ())
			{
				auto cluster (boost::static_pointer_cast <lambda_p::cluster> (results [0]));
				lambda_p_script_io::synthesizer synthesizer;
				std::vector <boost::shared_ptr <lambda_p::node>> arguments;
				std::vector <boost::shared_ptr <lambda_p::node>> results;
				arguments.push_back (cluster);
				synthesizer (errors_a, arguments, results);
				std::vector <llvm::Function *> functions;
				if (!(*errors_a) ())
				{
					auto cluster (boost::static_pointer_cast <lambda_p_script::cluster::node> (results [0]));
					if (!cluster->routines.empty ())
					{
						for (auto i (cluster->routines.begin ()), j (cluster->routines.end ()); i != j && ! (*errors_a) (); ++i)
						{
							auto signature_routine (*i);
							std::vector <boost::shared_ptr <lambda_p::node>> arguments;
							std::vector <boost::shared_ptr <lambda_p::node>> results;
							signature_routine->perform (errors_a, arguments, results);
							if (!(*errors_a) ())
							{
								if (results.size () == 1)
								{
									auto function_type (boost::dynamic_pointer_cast <lambda_p_llvm::function_type::node> (results [0]));
									if (function_type.get () != nullptr)
									{
										auto function (llvm::Function::Create (function_type->function_type (), llvm::GlobalValue::ExternalLinkage));
										two->module->getFunctionList ().push_back (function);
										functions.push_back (function);
									}
									else
									{
										(*errors_a) (L"Signature routine did not return a function_type");
									}
								}
								else
								{
									(*errors_a) (L"Signature routine did not return exactly one argument");
								}
							}
							++i;
							if (i == j)
							{
								(*errors_a) (L"Signature doesn't have an associated routine");
							}
						}
						size_t position (0);
						for (auto i (cluster->routines.begin () + 1), j (cluster->routines.end () + 0); i != j && ! (*errors_a) (); ++i, ++position)
						{
							auto signature_routine (*i);
							auto function (functions [position]);
							assert (function->getBasicBlockList ().size () == 0);
							auto block (llvm::BasicBlock::Create (function->getContext ()));
							function->getBasicBlockList ().push_back (block);
							analyzer.context.block->block = block;
							std::vector <boost::shared_ptr <lambda_p::node>> arguments;
							std::vector <boost::shared_ptr <lambda_p::node>> results;
							signature_routine->perform (errors_a, arguments, results);						
							if (!(*errors_a) ())
							{
								if (results.size () == 0)
								{
								}
								else
								{
									(*errors_a) (L"Body routine returned a value");
								}
							}
							++i;
							if (i == j)
							{
								(*errors_a) (L"Signature routine doesn't have associated body routine");
							}
						}
					}
				}
			}
		}
		else
		{
			invalid_type (errors_a, parameters [1], 1);
		}
	}
	else
	{
		invalid_type (errors_a, parameters [0], 0);
	}
}

size_t lambda_p_llvm::synthesizer::operation::count ()
{
	return 2;
}