#pragma once

#include <lambda_p/parser/production_target.h>

namespace lambda_p
{
	namespace parser
	{
		enum state_id
		{
			state_error,
			state_begin,
			state_routine,
			state_body,
			state_statement,
			state_result_ref,
			state_data,
			state_declaration,
			state_node_list
		};
		class state : public production_target
		{
		public:
			state (void);
			~state (void);
			virtual state_id state_type () = 0;
		};
	}
}

