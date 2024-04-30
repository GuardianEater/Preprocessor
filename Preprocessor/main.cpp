/*********************************************************************
 * file:   main.cpp
 * author: travis.gronvold (travis.gronvold@digipen.edu)
 * date:   March 25, 2024
 * Copyright © 2023 DigiPen (USA) Corporation. 
 * 
 * brief:  Entry point for the preproccessor
 *********************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <filesystem>

// reads in the contents of a file
std::string GetFileContents(const std::string& filename)
{
    std::filesystem::path parentDir = std::filesystem::current_path().parent_path(); // Get the current working directory
    std::filesystem::path clientDir = parentDir.append("Client").append(filename);
    std::ifstream file(clientDir);
    std::string content;

    if (file.is_open()) 
    {
        content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
    }
    else 
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
    }

    return content;
}

int main()
{
    std::string filename = "main.cpp";
    std::string contents = GetFileContents(filename);

    size_t containsHeader = contents.find_first_of("#include \"Reflection.h\"");

    if (containsHeader)
    {
        size_t pos = 0;
        size_t count = 0;

        // word to search for
        std::string searchstring = "printable";

        // scans the file for the given word
        while ((pos = contents.find(searchstring, pos)) != std::string::npos) 
        {
            // gets the value after the search word
            std::string valueType = contents.substr(pos + searchstring.length() + 1, contents.find_first_of(' ', pos + searchstring.length() + 1) - (pos + searchstring.length() + 1));
            std::cout << "Type: " << valueType << std::endl;

            pos += searchstring.length();
        }
    }
    else
    {
        std::cerr << "Reflection must be included: '#include \"Reflection.h\"' was not found" << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(30));

    return 0;
}
