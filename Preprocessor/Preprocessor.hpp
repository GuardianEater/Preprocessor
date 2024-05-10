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
#include <map>
#include <string>
#include <filesystem>
#include <iostream>

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

	public:

		// stores info about reflected types
		struct MetaInfo
		{
			std::string mType;
			std::string mVariableName;
			std::string mParentClass;
			std::string mFullClassPath;

			friend std::ostream& operator<<(std::ostream& os, const MetaInfo& info);

		};

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

		// creates a template in valid cpp code, if the mi's class path has already been written too will simply add to it
		inline void CreateTemplate(const MetaInfo& mi);

		// adds spaces to both sides of all occurences of 'padword'
		inline void AddPadding(std::string& fileContents, const std::string& padword) const;

		// finds the first that shows up in fileContents 
		inline size_t FindFirstString(const std::string& fileContents, const std::vector<std::string>& strings, size_t start = 0) const;

	private:
		std::string mFileContents;

		std::vector<std::string> mTokens;

		std::string mRemovedStrings;

		// this is the classPath mapped to the function of that class split up into lines
		std::map<std::string, std::list<std::string>> mClassMap;

		//std::vector<std::string> mRemovedStrings;
	};
} // namespace gep