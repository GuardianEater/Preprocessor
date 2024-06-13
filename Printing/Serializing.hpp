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
	}
}

template<typename Type>
inline void gep::json::File::Read(Type& item) const
{
	std::cout << "attempting to read an unknown object" << std::endl;
}

template<typename Type>
inline void gep::json::File::Write(const Type& item)
{
	std::cout << "attempting to write an unknown object" << std::endl;
}
