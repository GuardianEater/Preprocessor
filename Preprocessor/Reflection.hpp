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
#include <Printing.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// used prior to a variable declaration: serializable int Value;
/// enables the variable to be serialized using either read or write in a gep::json::file
#define serializable friend class gep::json::File;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// used prior to a variable declaration: serializable int Value;
/// enables the variable to be printed when using gep::print(...);
#define printable template<typename gep_detail_printer_type, typename gep_detail_void> friend struct gep::detail::Printer;