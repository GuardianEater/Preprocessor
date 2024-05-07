/*****************************************************************//**
 * \file   Serializer.hpp
 * \brief  Volatile file, this file will be modified when the preprocessor
 *         is ran.
 * 
 * \author 2018t
 * \date   May 2024
 *********************************************************************/

#include <fstream>
#include <iostream>
#include <filesystem>

class Serializer
{
public:
	Serializer(std::istream& input, std::ostream& output);

	Serializer(const std::filesystem::path& filePath);

private:

	// reads an item from the 
	template <typename Type>
	void Read(Type& item) const;

	template <typename Type>
	void Write(const Type& item);
};
