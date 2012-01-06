#include "synthesizer.h"

#include <lambda_p_script/synthesizer/routine.h>

lambda_p_script::synthesizer::synthesizer (boost::function <void (boost::shared_ptr <lambda_p_script::routine>)> target_a, boost::shared_ptr <lambda_p::errors::error_target> errors_a)
	: target (target_a),
	errors (errors_a)
{
}

void lambda_p_script::synthesizer::operator () (boost::shared_ptr <lambda_p::expression> expression_a)
{
	lambda_p_script::synthesizer::routine routine (*this, expression_a);
	
}