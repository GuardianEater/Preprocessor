/*****************************************************************//**
 * \file   main.hpp
 * \brief  
 * 
 * \author 2018t
 * \date   May 2024
 *********************************************************************/

#pragma once

#include <Reflection.hpp>

namespace client
{
	class /* odd place to put a commment */ TestObject
	{
	public:
		TestObject() = default;

	private:
		printable int mInt;
		printable double mDouble;
		printable size_t mSize;
		printable int mAnotherInt;
		printable int mEvenMoreData;
	};

	namespace math
	{
		class NotInt
		{
		public:
			NotInt() = default;
			NotInt(size_t data) : mData(data) {}

			size_t Get() const { return mData; }

		private:
			printable size_t mData;
		};
	}

	class LongTest{public:LongTest()=default;size_t GetValue()const{return(mValue);};private:size_t mValue=7;};

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

	class AnotherObject
	{
	public:
		// a nested object with 2 more datas inside
		class NestedObject 
		{
			using prair = std::pair<std::string, size_t>;
			using str = std::string;
		
			printable int mInnerData1;
			printable int mInnerData2;

			struct NestedStruct{int mDeeperData1;int mDeeperData2;};

			class NestedClass
			{
				printable int mDeeperData1;
				int mDeeperData2;
			};
		};

		void SetData1(int data)
		{
			mData1 = data;
		}

		void SetData2(int data)
		{
			mData2 = data;
		}

		void SetString(const std::string& str)
		{
			mString = str;
		}

	private:
		printable int mData1;
		printable int mData2;
		printable std::string mString;

	};namespace{const std::string david="hello";}class MtS{};class 
	StM 
	;
}

#include <.meta/main.meta>