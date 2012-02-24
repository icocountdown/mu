#pragma once

#include <core/segment.h>

#include <vector>

#include <boost/shared_ptr.hpp>

namespace mu
{
	namespace core
	{
		class node;
	}
}
namespace lambda_p_script
{
	namespace cluster
	{
		class node;
	}
	class context
	{
	public:
		context (boost::shared_ptr <lambda_p_script::cluster::node> cluster_a, boost::shared_ptr <mu::core::node> parameters_a, size_t size);
		boost::shared_ptr <mu::core::node> parameters;
		boost::shared_ptr <lambda_p_script::cluster::node> cluster;
		std::vector <std::vector <boost::shared_ptr <mu::core::node>>> nodes;
	};
}

