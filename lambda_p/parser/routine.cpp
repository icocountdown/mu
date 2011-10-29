#include <lambda_p/parser/routine.h>

#include <lambda_p/core/routine.h>
#include <lambda_p/core/association.h>
#include <lambda_p/binder/list_binder.h>
#include <lambda_p/parser/position_set.h>

#include <boost/bind.hpp>

lambda_p::parser::routine::routine (std::vector <std::pair <std::wstring, boost::shared_ptr <lambda_p::binder::node>>> & injected_parameters, std::vector <std::wstring> & injected_returns)
	: parsed_routine (false),
	routine_m (new lambda_p::core::routine),
	have_surface (false)
{
	for (auto i = injected_parameters.begin (); i != injected_parameters.end (); ++i)
	{
		size_t injected_position (routine_m->add_declaration ());
		positions [i->first] = injected_position;
		routine_m->injected_surface->declarations.push_back (injected_position);
		routine_m->injected [injected_position] = i->second;
	}
	for (auto i = injected_returns.begin (); i != injected_returns.end (); ++i)
	{
		size_t injected_position (routine_m->add_declaration ());
		routine_m->injected_surface->references.push_back (injected_position);
		unresolved_references.insert (std::multimap <std::wstring, boost::function <void (size_t)>>::value_type (*i, boost::bind <void> (lambda_p::parser::position_set (routine_m->injected_surface->references, routine_m->injected_surface->references.size () - 1), _1)));
	}
}

lambda_p::parser::state_id lambda_p::parser::routine::state_type ()
{
	return lambda_p::parser::state_routine;
}

boost::function <void (size_t)> lambda_p::parser::routine::sink_declaration ()
{
	routine_m->surface->declarations.push_back (~0);
	boost::function <void (size_t)> result (boost::bind <void> (lambda_p::parser::position_set (routine_m->surface->declarations, routine_m->surface->declarations.size () - 1), _1));
	return result;
}

boost::function <void (size_t)> lambda_p::parser::routine::sink_argument ()
{
	routine_m->surface->references.push_back (~0);
	boost::function <void (size_t)> result (boost::bind <void> (lambda_p::parser::position_set (routine_m->surface->references, routine_m->surface->references.size () - 1), _1));
	return result;
}