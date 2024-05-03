/*********************************************************************
 * file:   main.cpp
 * author: travis.gronvold (travis.gronvold@digipen.edu)
 * date:   March 25, 2024
 * Copyright © 2023 DigiPen (USA) Corporation. 
 * 
 * brief:  Entry point for the preproccessor
 *********************************************************************/

// std
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <filesystem>

// preprocessor
#include "Preprocessor.hpp"
#include <Printing.hpp>

#include <chrono>

// reads in the contents of a file
std::string GetFileContents(const std::filesystem::path& filePath)
{
    std::ifstream file(filePath);
    std::string content;

    if (file.is_open()) 
    {
        content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
    }
    else 
    {
        std::cerr << "Error: Unable to open file '" << filePath.filename() << "'" << " at '" << filePath.root_path() << "'" << std::endl;
    }

    return content;
}

int main()
{
    gep::Preprocessor preprocessor;

    std::filesystem::path targetFile = std::filesystem::current_path().parent_path().append("Client").append("main.cpp");

    preprocessor.PreprocessFile(targetFile);

    //// read the config file
    //    // created a json file reader
    //std::string filename = "main.cpp";

    //auto start = std::chrono::steady_clock::now();

    //std::string contents = GetFileContents(filename);

    //// End the clock
    //auto end = std::chrono::steady_clock::now();

    //// Calculate the duration
    //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    //// Output the duration
    //std::cout << "Time taken: " << duration.count() << " milliseconds" << std::endl;

    //size_t containsHeader = contents.find_first_of("#include \"Reflection.h\"");

    //if (containsHeader)
    //{
    //    size_t pos = 0;
    //    size_t count = 0;

    //    // word to search for
    //    std::string searchstring = "printable";

    //    // scans the file for the given word
    //    while ((pos = contents.find(searchstring, pos)) != std::string::npos) 
    //    {
    //        // gets the value after the search word
    //        std::string valueType = contents.substr(pos + searchstring.length() + 1, contents.find_first_of(' ', pos + searchstring.length() + 1) - (pos + searchstring.length() + 1));
    //        std::cout << "Type: " << valueType << std::endl;

    //        pos += searchstring.length();
    //    }
    //}
    //else
    //{
    //    std::cerr << "Reflection must be included: '#include \"Reflection.h\"' was not found" << std::endl;
    //}

    std::string wait;
    std::cin >> wait;

    return 0;
}
