//
//  statement_end.cpp
//  lambda_p
//
//  Created by Colin LeMahieu on 9/2/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <lambda_p/tokens/statement_end.h>

size_t lambda_p::tokens::statement_end::token_id ()
{
	return ::lambda_p::tokens::token_ids::token_id_statement_end;
}