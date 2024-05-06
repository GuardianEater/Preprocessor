/*****************************************************************//**
 * \file   main.cpp
 * \brief  
 * 
 * \author 2018t
 * \date   May 2024
 *********************************************************************/

#include <thread>
#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <list>
#include <string>
#include <fstream>
#include <filesystem>
#include <deque>

// printing
#include <Printing.hpp>

// reflection
#include <Reflection.h>

// random numbers
#include "Random.hpp"

// test object with 2 datas inside
class AnotherObject 
{
    // a nested object with 2 more datas inside
	class NestedObject 
    {
		int mInnerData1;
		int mInnerData2;
	};

	int mData1;
	int mDsata2;

};

class /* odd place to put a commment */ TestObject
{
public:
	TestObject() = default;

private:
	printable int mInt;
	printable double mDouble;
	printable size_t mSize;
	printable AnotherObject mObject;
	printable int mAnotherInt;
	printable std::vector<AnotherObject*> mSutff;
};

// allows loading a dictionary and choosing random values from it
class Dictionary
{
public:
    // empty dictionary
    Dictionary() : mWords(), mIsValid(false) {};

    // loads a dictionary given a path to a textfile of words
    Dictionary(const std::filesystem::path& dictionaryTextPath) : mWords(), mIsValid(false)
    {
        Load(dictionaryTextPath);
    }

    // loads a new dictionary given a path to a textfile of words
    void Load(const std::filesystem::path& dictionaryTextPath)
    {
        // opens a file
        std::ifstream inFile(dictionaryTextPath);
        if (!inFile.is_open())
        {
            std::cerr << "Failed to open file " << dictionaryTextPath.filename() << std::endl;
            std::cerr << "Path was: " << dictionaryTextPath << std::endl;
            return;
        }

        // reads in tokens out of a file
        while (inFile >> mWords.emplace_back());

        mIsValid = true;
    }

    // gives a random word from the dictionary
    const std::string& GetRandomWord() const
    {
        // checks if the dictionary has been loaded
        if (mIsValid)
        {
            return mWords.at(GetRandom(0ull, mWords.size() - 1));
        }

        std::cerr << "Attempted to get a random word before loading dictionary" << std::endl;
        return "";
    }

    std::vector<std::string> GetRandomWords(size_t min, size_t max = 0) const
    {
        // makes the upper bound always at least the lowerbound
        if (max < min) max = min;

        // checks if the dictionary has been loaded
        std::vector<std::string> randomWords;
        if (mIsValid)
        {
            for (size_t i = 0; i < GetRandom(min, max); i++) randomWords.push_back(GetRandomWord());
        }
        else
        {
            std::cerr << "Attempted to get a random words before loading dictionary" << std::endl;
        }

        return randomWords;
    }

private:
    // the words in this dictionary 
    std::vector<std::string> mWords;

    // whether or not the dictionary was loaded successfully
    bool mIsValid;
};

static void test_print()
{
    // loads 2 dictionarys for random words
    Dictionary regularDictionary;
    regularDictionary.Load("Assets\\words.txt");

    Dictionary namesDictionary;
    namesDictionary.Load("Assets\\names.txt");

    // double vector
    std::vector<std::vector<std::string>> vectors;
    for (int i = 0; i < 10; i++) vectors.push_back(namesDictionary.GetRandomWords(1, 7));

    // semi-complex map
    std::map<std::string, std::vector<std::string>> mapcontents;
    for (int i = 0; i < 10; i++) mapcontents.emplace(regularDictionary.GetRandomWord(), namesDictionary.GetRandomWords(1, 7));

    // vector of 10 names
    std::vector<std::string> names = namesDictionary.GetRandomWords(10);

    // simple set
    std::unordered_map<std::string, size_t> map;
    for (int i = 0; i < 10; i++) map.emplace(names[i], i);

    //          \
    /           \
    /           \
    testing various comment types

    
    /* cxomment // */ gep::Print(map); // another comment      \ fsd
    gep::Print("Should Print");
    // copment \
    gep::Print(names);
    gep::Print("lmao a // in a comment what are you going to do");
    gep::Print("lmao a /* in a comment what are you going to do");
    gep::Print("lmao a */ in a comment what are you going to do");


}

int main()
{
	std::cout << "Hello" << std::endl;

	test_print();

    std::string wait;
    std::cin >> wait;
}
