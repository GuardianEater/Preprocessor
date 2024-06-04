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

int main()
{
    gep::Preprocessor preprocessor;

    preprocessor.InitializeMetaHeader();

    std::filesystem::path targetFile1 = std::filesystem::current_path().parent_path().append("Client").append("main.hpp");

    int result = preprocessor.PreprocessFile(targetFile1);
    if (result)
    {
        std::string wait;
        std::cin >> wait;
    }
    return result;
}
