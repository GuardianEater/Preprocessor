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

// simdjson
#include <simdjson.h>

// allows pretty printing
#include <Printing.hpp>

// this
#include "Preprocessor.hpp"

#include "Reflection.h"

namespace gep
{
    Preprocessor::Preprocessor()
    {
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
        // opens a file
        std::ifstream inFile(path);
        if (!inFile.is_open())
        {
            std::cerr << "Failed to open file " << path.filename() << std::endl;
            std::cerr << "Path was: " << path << std::endl;
            return;
        }

        //reads in all of the data from the file
        std::string fileContent((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();

        // removes all of the comments from the file
        RemoveComments(fileContent);

        // checks if the file read in has the needed include
        if (!HasInclude(fileContent))
        {
            gep::Print("No reflection include was found");
            return;
        }
    }

    bool Preprocessor::HasInclude(const std::string& fileContents) const
    {
        // locates an include directive
        const std::string includeString("#include");

        // start searching for includes at the beginning of the file
        size_t includePos = 0;
        while (true)
        {
            // locate the next #include
            includePos = fileContents.find(includeString, includePos + 1);

            // if a #include was not sucessfully found
            if (includePos == std::string::npos) return false;

            // gets the start and end postion of the content inbetween the <> or ""
            size_t includeStart = fileContents.find_first_of("<\"", includePos + includeString.length()) + 1;
            size_t includeEnd   = fileContents.find_first_of(">\"", includeStart);

            // gets the string inbetween the <> or ""
            std::string includeContents = fileContents.substr(includeStart, includeEnd - includeStart);

            // checks if the string contains the name of the reflection file
            size_t reflectionPos = includeContents.find(GetReflectionFileName().c_str());

            // if the include pos 
            if (reflectionPos != std::string::npos)
            {
                // found Reflection.h
                return true;
            }

        }
    }

    void Preprocessor::RemoveComments(std::string& fileContents) const
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
        for (size_t i = 0; i < fileContents.length(); i++)
        {
            // case for comment start
            if (fileContents[i] == '/'             // could potentially be a comment
                && (i + 1 < fileContents.length()) // check if its the last character in the file
                && !inQuotes                       // check if currently in quotes
                && !(inCComment || inCppComment))  // check if currently inside of a comment)
            {
                // checks what type of comment it is
                if (fileContents[i + 1] == '*')
                {
                    inCComment = true;
                }
                else if (fileContents[i + 1] == '/')
                {
                    inCppComment = true;
                }
            }
            
            // case for quote start/end
            else if (fileContents[i] == '\"'      // checks if there is a quote
                && (i - 1 != sizeMax)             // checks if its the first character in a file
                && (fileContents[i - 1] != '\\')  // checks if the previous character is a '\'
                && !(inCComment || inCppComment)) // check if currently inside of a comment
            {
                // all of those check are true then we are entering a quote or exiting a quote
                inQuotes = !inQuotes;
            }

            // case for c-comment end
            else if (fileContents[i] == '*'
                && (i + 1 < fileContents.length()) // checks if last character in file
                && fileContents[i + 1] == '/'      // checks if the following character is a '/' making */
                && inCComment                      // must already be-inside a c-comment
                && !inCppComment                   // must not be in a cpp comment
                && !inQuotes)                      // must not be in quotes
            {
                // no longer in a comment
                inCComment = false;

                // remove the end of the c-comment
                fileContents[i] = ' ';
                fileContents[i + 1] = ' ';
            }

            // case for cpp-comment end
            else if (fileContents[i] == '\n'
                && (i - 1 != sizeMax) // checks if its the first character in a file
                && inCppComment       // must be iside of a cpp comment
                && !nextLineComment   // checks if the last character in the cpp comment '\'
                && !inCComment        // must not be in a c comment
                && !inQuotes)         // must not be in quotes
            {
                inCppComment = false;
            }

            // case for newline cpp-comment start
            else if (fileContents[i] == '\\'
                && inCppComment
                && !inCComment
                && !inQuotes)
            {
                nextLineComment = true;
            }

            // case for newline cpp-comment end
            else if (fileContents[i] != '\\'
                && nextLineComment)
            {
                nextLineComment = false;
            }

            // where the characters are actually deleted
            if (inCComment || inCppComment)
            {
                fileContents[i] = ' ';
            }
        }
    }
}
