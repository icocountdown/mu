#pragma once

#include <mu/core/segment.h>
#include <mu/script_runtime/context.h>

#include <boost/shared_ptr.hpp>

namespace mu
{
	namespace core
	{
		class node;
		namespace errors
		{
			class error_target;
		}
	}
}
namespace mu
{
	namespace script
	{
		bool check_count (mu::script_runtime::context & context_a, size_t expected);
		void invalid_type (mu::script_runtime::context & context_a, std::type_info const & actual, std::type_info const & expected, size_t position);
		template <typename type1_t=void, typename type2_t=void, typename type3_t=void, typename type4_t=void, typename type5_t=void>
		class check
		{		
		public:
			bool operator () (mu::script_runtime::context & context_a)
			{
				bool result (check_count (context_a, 5));
				if (result)
				{
					bool t0 (boost::dynamic_pointer_cast <type1_t> (context_a.parameters (0)).get () != nullptr);
					bool t1 (boost::dynamic_pointer_cast <type2_t> (context_a.parameters (1)).get () != nullptr);
					bool t2 (boost::dynamic_pointer_cast <type3_t> (context_a.parameters (2)).get () != nullptr);
					bool t3 (boost::dynamic_pointer_cast <type4_t> (context_a.parameters (3)).get () != nullptr);
					bool t4 (boost::dynamic_pointer_cast <type5_t> (context_a.parameters (4)).get () != nullptr);
					result = result && t0 && t1 && t2 && t3 && t4;
					if (!result)
					{
						if (!t0)
						{
							invalid_type (context_a, typeid (context_a.parameters (0)), typeid (type1_t), 0);
						}
						if (!t1)
						{
							invalid_type (context_a, typeid (context_a.parameters (1)), typeid (type2_t), 1);
						}
						if (!t2)
						{
							invalid_type (context_a, typeid (context_a.parameters (2)), typeid (type3_t), 2);
						}
						if (!t3)
						{
							invalid_type (context_a, typeid (context_a.parameters (3)), typeid (type4_t), 3);
						}
						if (!t4)
						{
							invalid_type (context_a, typeid (context_a.parameters (4)), typeid (type5_t), 4);
						}
					}
				}
				return result;
			}
		};
		template <typename type1_t, typename type2_t, typename type3_t, typename type4_t>
		class check <type1_t, type2_t, type3_t, type4_t, void>
		{		
		public:
			bool operator () (mu::script_runtime::context & context_a)
			{
				bool result (check_count (context_a, 4));
				if (result)
				{
					bool t0 (boost::dynamic_pointer_cast <type1_t> (context_a.parameters (0)).get () != nullptr);
					bool t1 (boost::dynamic_pointer_cast <type2_t> (context_a.parameters (1)).get () != nullptr);
					bool t2 (boost::dynamic_pointer_cast <type3_t> (context_a.parameters (2)).get () != nullptr);
					bool t3 (boost::dynamic_pointer_cast <type4_t> (context_a.parameters (3)).get () != nullptr);
					result = result && t0 && t1 && t2 && t3;
					if (!result)
					{
						if (!t0)
						{
							invalid_type (context_a, typeid (context_a.parameters (0)), typeid (type1_t), 0);
						}
						if (!t1)
						{
							invalid_type (context_a, typeid (context_a.parameters (1)), typeid (type2_t), 1);
						}
						if (!t2)
						{
							invalid_type (context_a, typeid (context_a.parameters (2)), typeid (type3_t), 2);
						}
						if (!t3)
						{
							invalid_type (context_a, typeid (context_a.parameters (3)), typeid (type4_t), 3);
						}
					}
				}
				return result;
			}
		};
		template <typename type1_t, typename type2_t, typename type3_t>
		class check <type1_t, type2_t, type3_t, void, void>
		{		
		public:
			bool operator () (mu::script_runtime::context & context_a)
			{
				bool result (check_count (context_a, 3));
				if (result)
				{
					bool t0 (boost::dynamic_pointer_cast <type1_t> (context_a.parameters (0)).get () != nullptr);
					bool t1 (boost::dynamic_pointer_cast <type2_t> (context_a.parameters (1)).get () != nullptr);
					bool t2 (boost::dynamic_pointer_cast <type3_t> (context_a.parameters (2)).get () != nullptr);
					result = result && t0 && t1 && t2;
					if (!result)
					{
						if (!t0)
						{
							invalid_type (context_a, typeid (context_a.parameters (0)), typeid (type1_t), 0);
						}
						if (!t1)
						{
							invalid_type (context_a, typeid (context_a.parameters (1)), typeid (type2_t), 1);
						}
						if (!t2)
						{
							invalid_type (context_a, typeid (context_a.parameters (2)), typeid (type3_t), 2);
						}
					}
				}
				return result;
			}
		};
		template <typename type1_t, typename type2_t>
		class check <type1_t, type2_t, void, void, void>
		{		
		public:
			bool operator () (mu::script_runtime::context & context_a)
			{
				bool result (check_count (context_a, 2));
				if (result)
				{
					bool t0 (boost::dynamic_pointer_cast <type1_t> (context_a.parameters (0)).get () != nullptr);
					bool t1 (boost::dynamic_pointer_cast <type2_t> (context_a.parameters (1)).get () != nullptr);
					result = result && t0 && t1;
					if (!result)
					{
						if (!t0)
						{
							invalid_type (context_a, typeid (context_a.parameters (0)), typeid (type1_t), 0);
						}
						if (!t1)
						{
							invalid_type (context_a, typeid (context_a.parameters (1)), typeid (type2_t), 1);
						}
					}
				}
				return result;
			}
		};
		template <typename type1_t>
		class check <type1_t, void, void, void, void>
		{
		public:
			bool operator () (mu::script_runtime::context & context_a)
			{
				bool result (check_count (context_a, 1));
				if (result)
				{
					bool t0 (boost::dynamic_pointer_cast <type1_t> (context_a.parameters (0)).get () != nullptr);
					result = result && t0;
					if (!result)
					{
						if (!t0)
						{
							invalid_type (context_a, typeid (context_a.parameters (0)), typeid (type1_t), 0);
						}
					}
				}
				return result;
			}
		};
		template <>
		class check <void, void, void, void, void>
		{
		public:
			bool operator () (mu::script_runtime::context & context_a)
			{
				bool result (check_count (context_a, 0));
				return result;
			}
		};
	}
}