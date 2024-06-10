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
#include <OutStream.hpp>

// random numbers
#include "Random.hpp"

// this
#include <main.hpp>

// 2 dictionarys
client::Dictionary gRegularDictionary;
client::Dictionary gNamesDictionary;

static void test_print()
{
	// double vector
	std::vector<std::vector<std::string>> vectors;
	for (int i = 0; i < 10; i++) vectors.push_back(gNamesDictionary.GetRandomWords(1, 7));

	// semi-complex map
	std::map<std::string, std::vector<std::string>> mapcontents;
	for (int i = 0; i < 10; i++) mapcontents.emplace(gRegularDictionary.GetRandomWord(), gNamesDictionary.GetRandomWords(1, 7));

	// vector of 10 names
	std::vector<std::string> names = gNamesDictionary.GetRandomWords(10);

	// simple set
	std::unordered_map<std::string, size_t> map;
	for (int i = 0; i < 10; i++) map.emplace(names[i], i);

	std::vector<int> ints;
	ints.push_back(1);
	ints.push_back(2);
	ints.push_back(3);
	
	///* cxomment // */ gep::Print(map) << std::endl; // another comment      \ fsd
	gep::Print("Should Prin", names) << std::endl;
	gep::Print("my map = ", map);
	gep::Print("omg multi type print = ", 54, " also eat my double ", 0.003);
	// the line below is also a comment
	//gep::Print(names);
	//gep::Print("lmao a // in a comment what are you going to ", names, " string") << std::endl;
	//gep::Print("lmao a /* in a comment what are you going to do") << std::endl;
	//gep::Print("lmao a */ in a comment what are you going to do") << std::endl;


}

static void test_template()
{
	std::cout << "Hello" << std::endl;

	gep::json::File file;

	int eight = 8;
	file.Read(eight);
	std::cout << std::endl;

	client::AnotherObject obj;

	obj.SetData1(42);
	obj.SetData2(7);
	obj.SetString("frfr");

	file.Read(obj);
	std::cout << std::endl;

	obj.SetData1(54);
	obj.SetData2(2);
	obj.SetString("ong");

	file.Read(obj);
	std::cout << std::endl;

	client::TestObject testObject;

	file.Read(testObject);
	std::cout << std::endl;

	client::AnotherObject::NestedObject nObj;
	file.Read(nObj);
	std::cout << std::endl;
}

static void test_cout()
{
	std::string helloText("hello");

	std::vector<std::string> strings = gRegularDictionary.GetRandomWords(10, 20);
}

int main()
{
	// loads 2 dictionarys for random words
	gRegularDictionary.Load("Assets\\words.txt");
	gNamesDictionary.Load("Assets\\names.txt");

	test_print();
	//test_template();
	//test_cout();
}
