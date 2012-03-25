#pragma once

#include <mu/script/operation.h>

#include <map>

namespace mu
{
	namespace core
	{
		class cluster;
		class routine;
		class expression;
		class reference;
	}
	namespace script
	{
		namespace cluster
		{
			class remap : public mu::script::operation
			{
			public:
				bool operator () (mu::script::context & context_a) override;
				void operator () (boost::shared_ptr <mu::core::cluster> source, boost::shared_ptr <mu::core::cluster> target);
				void operator () (boost::shared_ptr <mu::core::routine> source, boost::shared_ptr <mu::core::routine> target);
				void operator () (boost::shared_ptr <mu::core::expression> source, boost::shared_ptr <mu::core::expression> target);
				void operator () (boost::shared_ptr <mu::core::reference> source, boost::shared_ptr <mu::core::reference> target);
				std::map <boost::shared_ptr <mu::core::node>, boost::shared_ptr <mu::core::node>> mapping;
			};
		}
	}
}