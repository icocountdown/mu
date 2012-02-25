#pragma once

#include <mu/core/visitor.h>

#include <boost/shared_ptr.hpp>

#include <map>

namespace mu
{
	namespace script
	{
		class call;
		class routine;
		namespace cluster
		{
			class node;
		}
	}
	namespace script_io
	{
		class generator;
		class expression : public mu::core::visitor
		{
		public:
			expression (boost::shared_ptr <mu::script::cluster::node> cluster_a, std::map <boost::shared_ptr <mu::core::expression>, size_t> & reservations_a, boost::shared_ptr <mu::script::call> call_a, boost::shared_ptr <mu::core::node> node_a);
			void operator () (mu::core::expression * expression_a) override;
			void operator () (mu::core::parameters * parameters_a) override;
			void operator () (mu::core::reference * reference_a) override;
			void operator () (mu::core::node * node_a) override;
			void operator () (mu::core::routine * routine_a) override;
			boost::shared_ptr <mu::core::node> node;
			boost::shared_ptr <mu::script::call> call_m;
			boost::shared_ptr <mu::script::cluster::node> cluster;
			std::map <boost::shared_ptr <mu::core::expression>, size_t> & reservations;
		};
	}
}
