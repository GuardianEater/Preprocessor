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

#include <stack>

// simdjson
#include <simdjson.h>

// allows pretty printing
#include <Printing.hpp>

// this
#include "Preprocessor.hpp"

#include "Reflection.h"

#include "Timer.hpp"


namespace gep
{
    Preprocessor::Preprocessor()
    {
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

    void Preprocessor::CreateConfig()
    {
    }

    bool Preprocessor::HasConfig()
    {
        return false;
    }

    void Preprocessor::PreprocessFile(const std::filesystem::path& path)
    {
        // starts a timer to measure file process speed
        Timer timer;
        timer.Start();
        
        // reads in the data from the given file
        if (!ReadFile(path))
        {
            std::cerr << "Failed to open file: " << path.filename() << std::endl;
            std::cerr << "Path was: " << path << std::endl;

            return;
        }

        // removes all of the comments from the file
        RemoveComments();

        // checks if the file read in has the needed include
        if (!HasInclude())
        {
            std::cerr << "No reflection include was found for file: " << path.filename() << std::endl;
            return;
        }

        // masks all strings with '$'
        MaskStrings();

        // adds a bunch of padding and removes excess spaces
        NormalizeSpaces();

        // tokenizes the fileContents
        std::stringstream fCs(mFileContents);
        while (fCs >> mTokens.emplace_back());

        //gep::Print(mTokens);

        //size_t nameEnd = 0;
        //size_t stack = 0;
        //while (true)
        //{
        //    size_t closestStart = FindFirstString(fileContents, { "class", "namespace", "struct", "}"}, nameEnd);

        //    // if there are none of the search terms back out
        //    if (closestStart == std::string::npos) break;

        //    if (fileContents.at(closestStart) == '}')
        //    {
        //        stack--;
        //        nameEnd++;
        //        continue;
        //    }

        //    // finds the end of the found item
        //    size_t closestEnd = fileContents.find_first_of(" \n{", closestStart);

        //    // in the case of a nameless namespace do nothing
        //    if (fileContents.at(closestEnd) == '{')
        //    {
        //        nameEnd++;
        //        continue;
        //    }

        //    // gets the name after the found item
        //    size_t nameStart  = fileContents.find_first_not_of(" \n", closestEnd);
        //    nameEnd = fileContents.find_first_of("{;", nameStart);

        //    if (fileContents.at(nameEnd) == '{')
        //    {
        //        stack++;
        //    }
        //    std::string name = fileContents.substr(nameStart, nameEnd - nameStart);

        //    gep::Print(name) << std::endl;
        //    gep::Print(stack) << std::endl;
        //    
        //}

        // undoes MaskStrings()
        RestoreStrings();


        std::ofstream outFile("Generated\\normalized.cpp");
        gep::Print(mFileContents, outFile);
        outFile.close();

        gep::Print(timer);
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

    bool Preprocessor::HasInclude() const
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
            size_t reflectionPos = includeContents.find(GetReflectionFileName().c_str());

            // if the include pos 
            if (reflectionPos != std::string::npos)
            {
                // found Reflection.h
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
        constexpr size_t sizeMax = std::numeric_limits<size_t>::max();

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
                && (i - 1 != sizeMax)             // checks if its the first character in a file
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
                && (i - 1 != sizeMax) // checks if its the first character in a file
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
        AddPadding(mFileContents, ";");
        AddPadding(mFileContents, "{");
        AddPadding(mFileContents, "}");
        AddPadding(mFileContents, "(");
        AddPadding(mFileContents, ")");
        AddPadding(mFileContents, "namespace");
        AddPadding(mFileContents, "struct");
        AddPadding(mFileContents, "class");
        AddPadding(mFileContents, "=");
        // Note: cannot add a "\"" case because it would add spaces to the inside of a comment

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
    
    void Preprocessor::AddPadding(std::string& fileContents, const std::string& padword) const
    {
        size_t location = 0;
        while (true)
        {
            location = fileContents.find(padword, location);

            if (location == std::string::npos) break;

            // insert a space after
            fileContents.insert(location + padword.length(), " ");

            // insert a space before
            fileContents.insert(location, " ");

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
}
