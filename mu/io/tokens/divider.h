//
//  statement_end.h
//  lambda_p
//
//  Created by Colin LeMahieu on 9/2/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <mu/io/tokens/token.h>

#include <string>

namespace mu
{
	namespace io
	{
		namespace tokens
		{
			class divider : public mu::io::tokens::token
			{
				mu::string token_name () override;
				void operator () (mu::io::tokens::visitor * visitor_a) override;
			};
		}
	}
}