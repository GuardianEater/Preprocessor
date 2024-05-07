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

    return 0;
}
