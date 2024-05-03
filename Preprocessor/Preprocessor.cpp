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

// simdjson
#include <simdjson.h>

// allows pretty printing
#include <Printing.hpp>

// this
#include "Preprocessor.hpp"


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
        std::ifstream inFile(path);

        if (!inFile.is_open())
        {
            std::cerr << "Failed to open file " << path.filename() << std::endl;
            std::cerr << "Path was: " << path << std::endl;
            return;
        }

        std::set<std::string> tokens;
        std::string token;
        while (inFile >> token)
        {
            tokens.insert(token);
        }

        std::unordered_set<std::string> strings;
        strings.insert("save");
        strings.insert("billing");

        gep::Print(std::cout, tokens);
    }
}
