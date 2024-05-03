/*****************************************************************//**
 * \file   Preprocessor.hpp
 * \brief  the core of the preprocessor project
 * 
 * \author 2018t
 * \date   May 2024
 *********************************************************************/

#pragma once

// std
#include <vector> 
#include <list>
#include <string>
#include <filesystem>

/**
 * \brief designed to be ran on a visual studio project.
 */

namespace gep
{
	class Preprocessor
	{
	public: // construction

		// creates the preprocessor object
		Preprocessor();

		// destroys the proprocessor object
		~Preprocessor();



	public: // step 1: startup

		// reads a config if it exists
		void ReadConfig();

		// creates a config from a template
		void CreateConfig();

		// checks if a config file exists
		bool HasConfig();



	public: // step 2: 

		void PreprocessFile(const std::filesystem::path& path);



	private:
		std::list<std::string> mFiles;
	};
} // namespace gep