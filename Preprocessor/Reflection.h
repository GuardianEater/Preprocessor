/*********************************************************************
 * file:   Reflection.h
 * author: travis.gronvold (travis.gronvold@digipen.edu)
 * date:   March 25, 2024
 * Copyright © 2023 DigiPen (USA) Corporation. 
 * 
 * brief:  
 *********************************************************************/

#pragma once

// hide this

namespace gep
{
	std::string GetReflectionFileName()
	{
		static const std::filesystem::path path = __FILE__;

		return path.filename().string();
	}
}

// used prior to a variable declaration: printable int Value;
#define printable    // friend class gep::detail::Printer<int>;
#define serializable // friend class gep::Serializer;