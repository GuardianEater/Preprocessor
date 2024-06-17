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
	
	///* cxomment // */ gep::print(map) << std::endl; // another comment      \ fsd
	gep::print("All of the names ar: ", names, " and then ive got nested vectors of words: ", vectors) << std::endl;
	gep::print("my map = ", map, "your map = ", mapcontents);
	gep::print("omg multi type print = ", 54, " also eat my double ", 0.003);
	// the line below is also a comment
	//gep::print(names);
	//gep::print("lmao a // in a comment what are you going to ", names, " string") << std::endl;
	//gep::print("lmao a /* in a comment what are you going to do") << std::endl;
	//gep::print("lmao a */ in a comment what are you going to do") << std::endl;


}

static void test_template()
{
	std::cout << "-----" << __func__ << "-----\n" << std::endl;

	client::AnotherObject obj;
	obj.SetData1(1);
	obj.SetData2(2);
	obj.SetString("Hallo :D");

	client::TestObject testobj;
	testobj.mInt = 1;
	testobj.mAnotherInt = 2;
	testobj.mDouble = 0.13;
	testobj.mEvenMoreData = 42;
	testobj.mSize = 32;

	gep::print("another objec = ", obj, " meeeee = ", testobj, " end");


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

	//test_print();
	test_template();
	//test_cout();
}
