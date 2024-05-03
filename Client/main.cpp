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
#include <Reflection.h>

class AnotherObject 
{
	class NestedObject {
		int mInnerData1;
		int mInnerData2;
	};

	int mData1;
	int mDsata2;

};

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
