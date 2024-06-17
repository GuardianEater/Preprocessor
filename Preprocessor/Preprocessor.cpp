/*****************************************************************//**
 * \file   Preprocessor.cpp
 * \brief  
 * 
 * \author 2018t
 * \date   May 2024
 *********************************************************************/

// std
#include <unordered_set> 
#include <set>
#include <fstream>
#include <iostream>
#include <type_traits>
#include <regex>
#include <limits>
#include <functional>
#include <stack>

// simdjson
#include <simdjson.h>

// allows pretty printing
#include <Printing.hpp>

#include <OutStream.hpp>

// this
#include "Preprocessor.hpp"

#include "Timer.hpp"

#include <ShlObj.h>

namespace gep
{
    Preprocessor::Preprocessor()
    {
        // preallocate some space for strings 
        mRemovedStrings.reserve(1024llu);
    }

    Preprocessor::~Preprocessor()
    {
    }

    void Preprocessor::ReadConfig()
    {
        //simdjson::dom::parser parser;
        //simdjson::dom::element tweets = parser.load("pconfig.json");

    }

    void Preprocessor::CreateConfig() const
    {
    }

    bool Preprocessor::HasConfig() const
    {
        return false;
    }

    void Preprocessor::InitializeMetaHeader() const
    {
        // create the meta folder if it doesnt exist 
        std::filesystem::create_directory(".meta");

        //SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }

    void Preprocessor::GenerateIncludes() const
    {
        std::filesystem::create_directory("include");

        std::filesystem::path copyFrom = GetAppDataPath();
        std::filesystem::path pasteTo  = std::filesystem::current_path().append("include");

        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(copyFrom)) 
        {
            const std::filesystem::path& path = entry.path();
            if (entry.is_regular_file()) 
            {
                std::filesystem::copy(path, pasteTo.string() + "\\" + path.filename().string(), std::filesystem::copy_options::overwrite_existing);
                gep::cout << path.filename() << std::endl;
            }
        }

    }

    int Preprocessor::PreprocessFile(const std::filesystem::path& path)
    {
        // starts a timer to measure file process speed
        Timer timer;
        timer.Start();

        mFilePath = path;
        
        // reads in the data from the given file
        if (!ReadFile(path))
        {
            gep::cerr << "Failed to open file: " << path.filename() << std::endl;
            gep::cerr << "Path was: " << path << std::endl;

            return 1;
        }

        // removes all of the comments from the file
        RemoveComments();

        if (path.filename().extension() == ".cpp")
        {
            gep::cwar << "File: " << path.filename() << " is a cpp file, should this be a header?" << std::endl;
            gep::cwar << "Path was: " << path << std::endl;
        }

        // checks if the file read in has the needed include
        if (!HasInclude("Reflection.hpp"))
        {
            gep::cerr << "No reflection include was found for file: " << path.filename() << std::endl;
            return 1;
        }

        // now checks if the meta file is included at thee bottom of the file
        std::string metaFileName = mFilePath.filename().stem().string() + ".meta";
        if (!HasInclude(metaFileName))// reflection include was found
        {
            gep::cerr << "No meta include was found at the bottom of file "<< mFilePath.filename() << std::endl
                      << mFilePath.filename() << " must have \"#include <.meta/" << metaFileName << ">\" at the bottom of the file" << std::endl;
            return 1;
        }

        // masks all strings with '$'
        MaskStrings();

        // removes everything inbetween [[...]] including the square braces
        RemoveAttributeLists();

        // adds a bunch of padding and removes excess spaces
        NormalizeSpaces();

        //tokenizes the fileContents
        std::stringstream fCs(mFileContents);
        while (fCs >> mTokens.emplace_back());



        CollectMetaData();

        //gep::print(metaInfos);



        // undoes MaskStrings()
        RestoreStrings();

        // creates the files
        GenerateOutput();

        // empties variables for multiple calls
        Clear();
        gep::cout << "File: " << mFilePath.filename() << " completed in " + timer.AsString() << " seconds" << std::endl;

        return 0;
    }

    void Preprocessor::WriteFunctionDefinition(const MetaInfo& mi, const std::string& returnType, const std::string& functionPath, bool isConst)
    {
    }

    inline bool Preprocessor::ReadFile(const std::filesystem::path& path)
    {
        // opens a file
        std::ifstream inFile(path);
        if (!inFile.is_open()) return false;

        //reads in all of the data from the file
        mFileContents.assign((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();

        return true;
    }

    bool Preprocessor::HasInclude(const std::string& includeFile) const
    {
        // locates an include directive
        const std::string includeString("#include");

        // start searching for includes at the beginning of the file
        size_t includePos = 0;
        while (true)
        {
            // locate the next #include
            includePos = mFileContents.find(includeString, includePos + 1);

            // if a #include was not sucessfully found
            if (includePos == std::string::npos) return false;

            // gets the start and end postion of the content inbetween the <> or ""
            size_t includeStart = mFileContents.find_first_of("<\"", includePos + includeString.length()) + 1;
            size_t includeEnd   = mFileContents.find_first_of(">\"", includeStart);

            // gets the string inbetween the <> or ""
            std::string includeContents = mFileContents.substr(includeStart, includeEnd - includeStart);

            // checks if the string contains the name of the reflection file
            size_t reflectionPos = includeContents.find(includeFile);

            // if the include pos 
            if (reflectionPos != std::string::npos)
            {
                return true;
            }

        }

        return false;
    }

    void Preprocessor::MaskStrings()
    {
        bool inQuotes = false;
        bool prev = false;

        for (size_t i = 0; i < mFileContents.length(); i++)
        {
            prev = inQuotes;
            if (mFileContents[i] == '"' 
                && i != 0
                && mFileContents[i - 1] != '\\')
            {
                inQuotes = !inQuotes;
            }

            if (inQuotes && prev)
            {
                mRemovedStrings.push_back(mFileContents[i]);
                mFileContents[i] = '$';
            }
        }
    }

    inline void Preprocessor::RemoveAttributeLists()
    {
        size_t rightLocation = 0;
        size_t leftLocation = 0;

        while (true)
        {
            // locates left side
            leftLocation = mFileContents.find("[[", leftLocation);
            if (leftLocation == std::string::npos) break;

            // locates right side
            rightLocation = mFileContents.find("]]", leftLocation);
            if (rightLocation == std::string::npos) throw; // right location should always exist otherwise invalid syntax

            // deletes everything inbetween
            mFileContents.erase(leftLocation, (rightLocation - leftLocation) + 2);
        }
    }

    void Preprocessor::RestoreStrings()
    {
        bool inQuotes = false;
        bool prev = false;
        size_t currentIndex = 0;

        for (size_t i = 0; i < mFileContents.length(); i++)
        {
            prev = inQuotes;
            if (mFileContents[i] == '"'
                && i != 0
                && mFileContents[i - 1] != '\\')
            {
                inQuotes = !inQuotes;
            }

            if (inQuotes && prev)
            {
                mFileContents[i] = mRemovedStrings.at(currentIndex);
                currentIndex++;
            }
        }
    }

    // TODO: does not take into account charcters ie '"'
    void Preprocessor::RemoveComments()
    {
        // to determine whether or not im in a string, check the number of " that I have passed
        // if ive passed an even number im not in a string, if ive passed an odd number I am in a string

        // to count "
        // if the quote is in a comment, ignore all quotes after a // or inbetween /* and */, add edge cases as needed
        // 

        bool inQuotes = false;
        bool inCComment = false;
        bool inCppComment = false;
        bool nextLineComment = false;

        // loops through each character in the file
        for (size_t i = 0; i < mFileContents.length(); i++)
        {
            // case for comment start
            if (mFileContents[i] == '/'             // could potentially be a comment
                && (i + 1 < mFileContents.length()) // check if its the last character in the file
                && !inQuotes                        // check if currently in quotes
                && !(inCComment || inCppComment))   // check if currently inside of a comment)
            {
                // checks what type of comment it is
                if (mFileContents[i + 1] == '*')
                {
                    inCComment = true;
                }
                else if (mFileContents[i + 1] == '/')
                {
                    inCppComment = true;
                }
            }
            
            // case for quote start/end
            else if (mFileContents[i] == '\"'     // checks if there is a quote
                && (i != 0)                       // checks if its the first character in a file
                && (mFileContents[i - 1] != '\\') // checks if the previous character is a '\'
                && !(inCComment || inCppComment)) // check if currently inside of a comment
            {
                // all of those check are true then we are entering a quote or exiting a quote
                inQuotes = !inQuotes;
            }

            // case for c-comment end
            else if (mFileContents[i] == '*'
                && (i + 1 < mFileContents.length()) // checks if last character in file
                && mFileContents[i + 1] == '/'      // checks if the following character is a '/' making */
                && inCComment                       // must already be-inside a c-comment
                && !inCppComment                    // must not be in a cpp comment
                && !inQuotes)                       // must not be in quotes
            {
                // no longer in a comment
                inCComment = false;

                // remove the end of the c-comment
                mFileContents[i] = ' ';
                mFileContents[i + 1] = ' ';
            }

            // case for cpp-comment end
            else if (mFileContents[i] == '\n'
                && (i != 0)           // checks if its the first character in a file
                && inCppComment       // must be iside of a cpp comment
                && !nextLineComment   // checks if the last character in the cpp comment '\'
                && !inCComment        // must not be in a c comment
                && !inQuotes)         // must not be in quotes
            {
                inCppComment = false;
            }

            // case for newline cpp-comment start
            else if (mFileContents[i] == '\\'
                && inCppComment
                && !inCComment
                && !inQuotes)
            {
                nextLineComment = true;
            }

            // case for newline cpp-comment end
            else if (mFileContents[i] != '\\'
                && nextLineComment)
            {
                nextLineComment = false;
            }

            // where the characters are actually deleted
            if (inCComment || inCppComment)
            {
                mFileContents[i] = ' ';
            }
        }
    }

    void Preprocessor::NormalizeSpaces()
    {
        // adds padding to a bunch of differnt strings to aid in tokenization
        AddPadding(";");
        AddPadding("{");
        AddPadding("}");
        AddPadding("(");
        AddPadding(")");
        AddPadding(",");
        AddPadding("*");
        AddPadding("&");
        AddPadding("=");
        AddPadding("[");
        AddPadding("]");

        // not necessary but should in theory make tokenization faster
        RemoveExtraSpaces();
    }

    void Preprocessor::RemoveExtraSpaces()
    {
        // erases unnecessary spaces
        char previous = '\0';
        for (size_t i = 0; i < mFileContents.size();)
        {
            // if the previous and current character is a space remove one
            if ((previous == ' ' || previous == '\t') && mFileContents[i] == ' ')
            {
                mFileContents.erase(i, 1);
                continue;
            }

            // update previous
            previous = mFileContents[i];
            i++;
        }
    }

    inline void Preprocessor::WriteLine(const MetaInfo& mi, size_t lineNumber, const std::string& line)
    {
        std::list<std::string>& lines = mClassMap[mi.mFullClassPath];

        lines.insert(std::next(lines.begin(), lineNumber), line);
    }

    void Preprocessor::BuildPrinterTemplate(const MetaInfo& mi)
    {
        // if it already exists
        bool existingClass = mClassMap.contains(mi.mFullClassPath);

        std::string payload = "      gep::detail::build_and_run_printer(os, indent + 2, item." + mi.mVariableName + ") << std::endl;";

        // write a print for the variable
        if (existingClass)
        {
            WriteLine(mi, 6, payload);
        }
        // otherwise generate a function for the entire class
        else
        {
            WriteLine(mi, 0, "template<>struct gep::detail::Printer<"+mi.mFullClassPath+"> ");
            WriteLine(mi, 1, "{");
            WriteLine(mi, 2, "  static std::ostream& basic_print(std::ostream& os, size_t indent, const "+mi.mFullClassPath+"& item)");
            WriteLine(mi, 3, "  {");
            WriteLine(mi, 4, "      gep::detail::out_color(\"{\", os, indent, color::GREEN) << std::endl;");
            WriteLine(mi, 5,        payload);
            WriteLine(mi, 6, "      gep::detail::out_color(\"}\", os, indent, color::GREEN) << std::endl;");
            WriteLine(mi, 7, "      return os;");
            WriteLine(mi, 8, "  }");
            WriteLine(mi, 9, "};");
        }
    }

    inline void Preprocessor::BuildSerializingTemplate(const MetaInfo& mi)
    {
        // if it already exists
        bool existingClass = mClassMap.contains(mi.mFullClassPath);

        std::list<std::string>& lines = mClassMap[mi.mFullClassPath];

        auto write_FunctionDefinition = [&]() -> void
            {
                lines.push_back(std::string("template<> ") + "inline void gep::json::File::Read(" + mi.mFullClassPath + "& item) const" + "{");
            };

        auto write_Header = [&]() -> void
            {
                lines.push_back(std::string("std::cout << \"") + mi.mFullClassPath + "\" << \":\" << std::endl;");
            };

        auto write_Contents = [&](size_t location) -> void
            {
                lines.insert(std::next(lines.begin(), location), std::string("std::cout << \"") + mi.mVariableName + " = \" << item." + mi.mVariableName + " << \":\" << std::endl;");
            };

        auto write_Finish = [&]() -> void
            {
                lines.push_back("}");
            };

        if (existingClass)
        {
            write_Contents(2);
        }
        else
        {
            write_FunctionDefinition();
            write_Header();
            write_Contents(2);
            write_Finish();
        }

        //std::string result;
        //// converts the list into a string
        //for (const std::string& line : lines) result += "\n" + line;

        //return result;

    }

    void Preprocessor::AddPadding(const std::string& padword)
    {
        size_t location = 0;
        while (true)
        {
            location = mFileContents.find(padword, location);
            if (location == std::string::npos) break;

            // insert a space after
            mFileContents.insert(location + padword.length(), " ");

            // insert a space before
            mFileContents.insert(location, " ");

            location += 2;
        }
    }

    size_t Preprocessor::FindFirstString(const std::string& fileContents, const std::vector<std::string>& strings, size_t start) const
    {
        size_t found = std::string::npos;

        for (size_t i = 0; i < strings.size(); i++)
        {
            size_t index = fileContents.find(strings[i], start);

            if (index < found)
            {
                found = index;
            }
        }

        return found;
    }

    inline void Preprocessor::GenerateOutput() const
    {
        // the meta directory should exist becuase of the initialization call, then create the meta file
        std::string metaFileName = mFilePath.filename().stem().string() + ".meta";
        std::ofstream outFile(".meta\\" + metaFileName);

        // adds pragma once for safe keeping and the include for the definition
        outFile << "#pragma once\n";

        // loops through all of the print objects writing the function out to the file for each
        for (const auto& lines : mClassMap)
        {
            for (const auto& line : lines.second)
            {
                outFile << line << "\n";
            }
        }
    }

    inline void Preprocessor::CollectMetaData()
    {
        // the keywords that are recognized
        const std::unordered_set<std::string> metaKeyWords = { "printable", "serializable" };

        // helpers to maintain scope
        const std::unordered_set<std::string> structureKeyWords = { "class", "union", "struct" };

        // the collected meta info from each variable
        std::vector<MetaInfo> metaInfos;

        // creates a MetaInfo vector
        for (auto tokenIt = mTokens.begin(); tokenIt != mTokens.end(); ++tokenIt)
        {
            // locates the tokenIt if it exists
            auto currentStructure = structureKeyWords.find(*tokenIt);
            if (currentStructure != structureKeyWords.end())
            {
                GetClassMetaData(tokenIt, "");
            }
        }
    }

    inline void Preprocessor::GetClassMetaData(std::vector<std::string>::iterator& tokenIt, const std::string& currentPath)
    {
        ClassMetaInfo mi;
        // moves to the next tokenIt
        ++tokenIt;

        // current class name
        mi.mName = *tokenIt;

        // current class path
        mi.mPath = currentPath + "::" + mi.mName;

        // move onwards
        ++tokenIt;

        // if this is encountered this is a class declaration nothing more can be gained
        if (*tokenIt == ";")
        {
            ++tokenIt;
            return;
        }

        // if has final move past it
        if (*tokenIt == "final")
        {
            ++tokenIt;
            mi.mIsFinal = true;
        }

        // this class derives from another class
        if (*tokenIt == ":")
        {
            do
            {
                // move passed the ":"
                ++tokenIt;

                // tokenIt is currently the base classes name
                mi.mBaseClasses.push_back(*tokenIt);

                ++tokenIt;
                // tokenIt is currently "," if there is more than one derived class
            } while (*tokenIt == ",");
        }

        // at this point it is guarenteed to have {...};
        // tokenIt is on "{"
        
        GetClassMemberMetaData(tokenIt, mi);
    }

    inline void Preprocessor::GetClassMemberMetaData(std::vector<std::string>::iterator& tokenIt, ClassMetaInfo& mi)
    {
        tokenIt++;

        // keywords that can be encountered in a class declaration
        std::unordered_map<std::string, bool> known =
        {
            { "const",     false },
            { "static",    false },
            { "virtual",   false },
            { "enum",      false },
            { "class",     false },
            { "struct",    false },
            { "union",     false },
            { "typedef",   false },
            { "using",     false },
            { "unsigned",  false },
            { "long",      false },
            { "short",     false },
            { "public",    false },
            { "private",   false },
            { "protected", false },
            { "inline",    false },
            { "&",         false },
            { "*",         false },
            { "friend",    false }
        };

        bool firstUnknownFound = false;
        bool secondUnknownFound = false;
        std::string type = "";

        while (true)
        {
            // this is for the constructor/destructor
            if ((mi.mName == *tokenIt) 
                || ("~" + mi.mName) == *tokenIt)
            {
                // parse function arguments
                while (*tokenIt != ";") tokenIt++;
                tokenIt++; // move past the semi-colon
                continue;
            }

            // checks if its a keyword that is skippable, also marks if the keyword was skipped
            if (known.contains(*tokenIt))
            {
                known.at(*tokenIt) = true;
                continue;
            }


            if (type.empty())
            {
                // the first unknown is the type
                type = *tokenIt;
                continue;
            }

            // the second unknown is the name
            std::string name = *tokenIt; 
                
            bool isArray = false;  // usually not an array
            size_t dimensions = 0; // usually has not dimensions

            // move to the first token after the name
            tokenIt++;

            // case for arrays
            if (*tokenIt == "[")
            {
                isArray = true;

                // will move the tokenIt until the end of the square brackets
                size_t stack = 1;
                while (stack)
                {
                    tokenIt++;

                    // keeps track of the stack of square brackets
                    if (*tokenIt == "]") { --stack; ++dimensions; }
                    else if (*tokenIt == "[") ++stack;
                }

                // move to the first token after the array
                tokenIt++;
            }

            // case for equals
            if (*tokenIt == "=")
            {
                if (isArray)
                {
                    // moves past all of the values
                    size_t stack = 1;
                    while (stack)
                    {
                        tokenIt++;

                        // keeps track of the stack of squiggly braces
                        if (*tokenIt == "}") --stack;
                        else if (*tokenIt == "{") ++stack;
                    }

                    // move to the first token after the array
                    tokenIt++;
                }
                else
                {
                    tokenIt += 2; // need to move past the = and value
                }
            }

            // case for semicolons or commas these are where variables are actually created
            if (*tokenIt == ";" || *tokenIt == ",")
            {
                VariableMetaInfo& vi = mi.CreateVariable();
                vi.mName = name;
                vi.mType = type;
                vi.mIsConst = known.at("const");
                vi.mIsStatic = known.at("static");
                vi.mIsPointer = known.at("*");
                vi.mIsReference = known.at("&");
                vi.mDimensions = dimensions;
                vi.mIsArray = isArray;

                // reset the map and continue looking through the class
                if (*tokenIt == ";")
                {
                    type.clear();
                }

                for (auto& pair : known) pair.second = false;
                tokenIt++;
                continue;
            }


            // ;  ---
            // ,  ---
            // [] ---
            // =  ---
            // (
            
        }
    }

    inline void Preprocessor::Clear()
    {
        mClassMap.clear();
        mRemovedStrings.clear();
        mTokens.clear();
    }

    inline std::filesystem::path Preprocessor::GetAppDataPath() const
    {
        std::string folderName = "gep";
        std::string programName = "preprocessor";

        PWSTR pathTemp;

        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_ProgramData, 0, nullptr, &pathTemp)))
        {
            std::wstring userPath = pathTemp;

            CoTaskMemFree(pathTemp);

            return std::filesystem::path(userPath.begin(), userPath.end()).append(folderName).append(programName);
        }

        gep::cerr << "Failed to locate users appdata folder";

        return std::filesystem::path();
    }

    inline void Preprocessor::CreateAppDataFolder() const
    {
        std::string folderName = "gep";
        std::string programName = "preprocessor";

        std::filesystem::path programAppDataPath = GetAppDataPath();

        if (!std::filesystem::exists(programAppDataPath))
        {
            std::filesystem::create_directories(programAppDataPath);
        }
    }

    std::ostream& operator<<(std::ostream& os, const Preprocessor::MetaInfo& info)
    {
        os << "Type = "  << info.mType          << std::endl;
        os << "Name = "  << info.mVariableName  << std::endl;
        os << "Path = "  << info.mFullClassPath << std::endl;
        os << "Class = " << info.mParentName    << std::endl;

        return os;
    }
}
