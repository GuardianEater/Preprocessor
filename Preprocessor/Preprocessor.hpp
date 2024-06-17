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
			std::string mType;		    // int, double, float, etc.
			std::string mVariableName;  // the name of the variable
			std::string mParentName;    // the name of the class that the variable is inside of
			std::string mFullClassPath; // the path of the class that the variable is inside of if the class is nested
			std::string mParentType;    // the type of the structure that the variable is inside of, struct, class, union
			std::string mKeyWord;		// ie, printable, serializable...

			friend std::ostream& operator<<(std::ostream& os, const MetaInfo& info);

		};

		struct VariableMetaInfo
		{
			VariableMetaInfo()
				: mName("")
				, mType("")
				, mAccess("")
				, mDimensions(0)
				, mIsArray(false)
				, mIsStatic(false)
				, mIsConst(false)
				, mIsPointer(false)
				, mIsReference(false)
			{}

			std::string mName;   // the name of the variable
			std::string mType;   // the type of the variable
			std::string mAccess; // public, protected, private

			size_t mDimensions;  // the amount of arrays this variable has: "int arr1[5][6][7]" has dimension of 3

			bool mIsArray;     // if an array
			bool mIsStatic;    // if static 
			bool mIsConst;     // if const
			bool mIsPointer;   // if a pointer
			bool mIsReference; // if a reference
		};

		struct FunctionMetaInfo
		{
			FunctionMetaInfo()
				: mName("")
				, mReturnType("")
				, mAccess("")
				, mIsVirtual(false)
				, mIsConst(false)
				, mIsInline(false)
			{}

			std::string mName;       // the name of the function
			std::string mReturnType; // the return type of the function
			std::string mAccess;     // public, protected, private

			bool mIsVirtual; // if the function is virtual
			bool mIsConst;   // if the function is const
			bool mIsInline;  // if the function is inline
			
			std::vector<VariableMetaInfo> mArguments; // all of the variables that the function takes as arguments
		};


		struct ClassMetaInfo
		{
			ClassMetaInfo()
				: mName("")
				, mPath("")
				, mType("")
				, mIsFinal(false)
			{}
			
			// createss a class member function
			FunctionMetaInfo& CreateFunction()
			{
				return mFunctions.emplace_back();
			}

			// creates a class member variable
			VariableMetaInfo& CreateVariable()
			{
				return mVariables.emplace_back();
			}

			// creates a nested class
			ClassMetaInfo& CreateNestedClass()
			{
				return mClasses.emplace_back();
			}

			std::string mName; // the name of this class
			std::string mPath; // the path to this class if it is nested
			std::string mType; // either struct, class, or union

			bool mIsFinal;     // if the class is final

			std::vector<std::string> mBaseClasses;    // the base classes if this inherits from other classes
		private:

			std::vector<FunctionMetaInfo> mFunctions;
			std::vector<VariableMetaInfo> mVariables; // the variables contained inside this class
			std::vector<ClassMetaInfo> mClasses;
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

		inline void RemoveAttributeLists();

		// restores all of the '$' added by MaskStrings
		inline void RestoreStrings();

		// removes all comments from the internal buffer
		inline void RemoveComments();

		// adds padding and removes extra spaces
		inline void NormalizeSpaces();

		// removes all duplicate spaces
		inline void RemoveExtraSpaces();

		// adds spaces to both sides of all occurences of 'padword'
		// if a padword is in a variable it will break it.
		inline void AddPadding(const std::string& padword);

		// finds the first that shows up in fileContents 
		inline size_t FindFirstString(const std::string& fileContents, const std::vector<std::string>& strings, size_t start = 0) const;

		// outputs all of the data inside of the class map to a file
		inline void GenerateOutput() const;

		inline void CollectMetaData();

		// where a class token is found pass in the class token
		inline void GetClassMetaData(std::vector<std::string>::iterator& tokenIt, const std::string& currentPath);

		// where a class "{" is found
		inline void GetClassMemberMetaData(std::vector<std::string>::iterator& tokenIt, ClassMetaInfo& currentClass);

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