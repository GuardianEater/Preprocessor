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
	public:

	public: // construction

		// creates the preprocessor object
		Preprocessor();

		// destroys the proprocessor object
		~Preprocessor();



	public: // step 1: startup

		// reads a config if it exists
		void ReadConfig();

		// creates a config from a template
		void CreateConfig() const;

		// checks if a config file exists
		bool HasConfig() const;

		// will create the meta header if it doesnt exist or will, clear an existing one. make sure to do this prior to preprocessing
		void InitializeMetaHeader() const;

		// generates the needed interface files
		void GenerateIncludes() const;


	public: // step 2: 

		// returns exit code for whether a file was preprocessed correctly
		int PreprocessFile(const std::filesystem::path& path);

	public:

		// stores info about reflected types
		struct MetaInfo
		{
			std::string mType;
			std::string mVariableName;
			std::string mParentName;
			std::string mFullClassPath;
			std::string mParentType;
			std::string mKeyWord; // ie, printable, serializable...

			friend std::ostream& operator<<(std::ostream& os, const MetaInfo& info);

		};

	private: // helpers for printing template code

		void WriteFunctionDefinition(const MetaInfo& mi, const std::string& returnType, const std::string& functionPath, bool isConst);

		// writes a line to the internal template 
		inline void WriteLine(const MetaInfo& mi, size_t lineNumber, const std::string& line);

		// creates a template in valid cpp code, if the mi's class path has already been written too will simply add to it
		inline void BuildPrinterTemplate(const MetaInfo& mi);

		// given meta info will create a template for serialization
		inline void BuildSerializingTemplate(const MetaInfo& mi);

	private:
		// reads the given file into a buffer
		inline bool ReadFile(const std::filesystem::path& path);

		// helper for PreprocessFile, determines if the current file has the specified include
		inline bool HasInclude(const std::string& includedFile) const;

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

		// outputs all of the data inside of the class map to a file
		inline void GenerateOutput() const;

		inline void CollectMetaData();

		// empties most member variables
		inline void Clear();

		// depricates this is handled by inno
		inline void CreateAppDataFolder() const;

		// gets the programs app data
		inline std::filesystem::path GetAppDataPath() const;

	private:
		std::string mFileContents;

		std::filesystem::path mFilePath;

		std::vector<std::string> mTokens;

		std::string mRemovedStrings;

		// this is the classPath mapped to the function of that class split up into lines
		std::map<std::string, std::list<std::string>> mClassMap;

		//std::vector<std::string> mRemovedStrings;
	};
} // namespace gep