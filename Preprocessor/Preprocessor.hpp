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
		// reads the given file into a buffer
		inline bool ReadFile(const std::filesystem::path& path);

		// helper for PreprocessFile, determines if the file being preprocessed has the reflection include
		inline bool HasInclude() const;

		// replaces all user defined character strings with '$'
		inline void MaskStrings();

		// restores all of the '$' added by MaskStrings
		inline void RestoreStrings();

		// removes all comments from the internal buffer
		inline void RemoveComments();

		// adds padding and removes extra spaces
		inline void NormalizeSpaces();

		// removes all duplicate spaces
		inline void RemoveExtraSpaces();

		// adds spaces to both sides of all occurences of 'padword'
		inline void AddPadding(std::string& fileContents, const std::string& padword) const;

		// finds the first that shows up in fileContents 
		inline size_t FindFirstString(const std::string& fileContents, const std::vector<std::string>& strings, size_t start = 0) const;

	private:
		std::string mFileContents;

		std::vector<std::string> mTokens;

		std::string mRemovedStrings;

		//std::vector<std::string> mRemovedStrings;
	};
} // namespace gep