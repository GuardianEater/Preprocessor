/*********************************************************************
 * file:   Reflection.h
 * author: travis.gronvold (travis.gronvold@digipen.edu)
 * date:   March 25, 2024
 * Copyright © 2023 DigiPen (USA) Corporation. 
 * 
 * brief:  
 *********************************************************************/

#pragma once

#include <Serializing.hpp>

// used prior to a variable declaration: printable int Value;
#define printable    friend class gep::json::File;
#define serializable // friend class gep::Serializer;