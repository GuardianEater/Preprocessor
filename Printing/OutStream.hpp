/*****************************************************************//**
 * \file   OutStream.hpp
 * \brief  wrapper for ostream, behaves identically to std::cout just
 *         a proof of concept
 * 
 * \author 2018t
 * \date   June 2024
 *********************************************************************/

#include <streambuf>
#include <iostream>

#include <Printing.hpp>

namespace gep
{
	class streambuf : public std::streambuf
	{
	public:
		streambuf(const std::string& colorCode = "") : std::streambuf(), mBuffer(), mColorCode(colorCode)
		{

		}

		const std::string& buffer() const
		{
			return mBuffer;
		}

	private:
		std::string mBuffer;
		std::string mColorCode;

		virtual int overflow(int c = -1) override
		{
			// if c is -1 it is end of file
			if (c != -1)
			{
				// writes to the buffer
				mBuffer += static_cast<char>(c);				
			}

			return c;
		}

		virtual int sync() override
		{
			std::cout << mColorCode << mBuffer << gep::color::RESET;
			mBuffer.clear();
			return 0;
		}
	};

	class ostream : public std::ostream
	{
	public:
		ostream(const std::string& colorCode = "") : mStreamBuffer(colorCode), std::ostream(&mStreamBuffer)
		{

		}

		std::string get_buffer() const
		{
			return mStreamBuffer.buffer();
		}

	private:
		gep::streambuf mStreamBuffer;
	};

	static gep::ostream cout(gep::color::GREEN);  // std::cout but green
	static gep::ostream cwar(gep::color::YELLOW); // std::cout but yellow
	static gep::ostream cerr(gep::color::RED);    // std::cerr but red
}
