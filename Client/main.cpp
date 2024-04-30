/*********************************************************************
 * file:   main.cpp
 * author: travis.gronvold (travis.gronvold@digipen.edu)
 * date:   March 25, 2024
 * Copyright © 2023 DigiPen (USA) Corporation. 
 * 
 * brief:  Example of how to use the reflection
 *********************************************************************/

#include <thread>
#include <iostream>
#include <vector>
#include <Reflection.h>

class AnotherObject {};

class TestObject
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

int main()
{
	std::cout << "Hello" << std::endl;

	std::this_thread::sleep_for(std::chrono::seconds(30));
}
