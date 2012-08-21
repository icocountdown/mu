#pragma once

namespace mu
{
    namespace core
    {
        namespace errors
        {
            class error_target;
        }
    }
    namespace script
    {
        namespace ast
        {
            class cluster;
        }
        namespace cluster
        {
            class node;
        }
        namespace synthesizer
        {
            class cluster
            {
            public:
                cluster (mu::core::errors::error_target & errors_a, mu::script::ast::cluster * cluster_a);
                mu::script::cluster::node * cluster_m;
                mu::core::errors::error_target & errors;
            };
        }
    }
}