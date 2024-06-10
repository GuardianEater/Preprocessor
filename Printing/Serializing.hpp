/*****************************************************************//**
 * \file   Serializing.hpp
 * \brief  
 * 
 * \author 2018t
 * \date   May 2024
 *********************************************************************/

#include <string>
#include <iostream>

namespace gep
{
	namespace json
	{
		class File
		{
		public:
			// reads an item from the 
			template <typename Type>
			void Read(Type& item) const;

			template <typename Type>
			void Write(const Type& item);
		};

		//template<typename Outer, typename Inner>
		//struct is_nested_type {
		//	template<typename T>
		//	static std::true_type test(typename T::Inner*);

		//	template<typename>
		//	static std::false_type test(...);

		//	static constexpr bool value = decltype(test<Outer>(nullptr))::value;
		//};
	}
}

template<typename Type>
inline void gep::json::File::Read(Type& item) const
{
	std::cout << "Read a basic object" << std::endl;
}