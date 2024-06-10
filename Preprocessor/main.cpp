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
#include <OutStream.hpp>

#include <chrono>

int main(int argc, char** argv)
{
    // initializes the current console for some reason
    //system("");

    // reads in all of the program arguments skipping 0, the program name
    std::vector<std::string> arguments;
    for (int i = 1; i < argc; i++)
    {
        arguments.push_back(argv[i]);
    }

    // create the preprocessor object
    gep::Preprocessor preprocessor;

    // intialize the preprocessor
    preprocessor.InitializeMetaHeader();

    // preprocess all of the arguments
    for (const std::string& argument : arguments)
    {
        // checks if the cureent argument is a command
        if (argument[0] == '-')
        {
            if (argument == "-getfiles")
            {
                gep::cout << "Grabbing include files..." << std::endl;
                preprocessor.GenerateIncludes();
            }
        }
        else
        {
            preprocessor.PreprocessFile(argument);
        }
    }

    return 0;
}
