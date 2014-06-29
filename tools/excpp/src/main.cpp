// Copyright (c) 2014, 임경현 (dlarudgus20)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
 * @file main.cpp
 * @brief EXtended C/C++ Preprocessor
 * @author dlarudgus20
 * @copyright The BSD (2-Clause) License
 */

#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <map>
#include <utility>

void DoProcess();
std::wstring FindKeyString(const std::wstring &str);

std::wifstream input;
std::wstringstream output;

int main(int argc, char *argv[])
{
	// parse argv and open files

	if (argc == 3)
	{
		input.open(argv[1]);
		if (!input.good())
		{
			std::cerr << "error: cannot open input file" << std::endl;
			return EXIT_FAILURE;
		}
	}
	else
	{
		std::cerr << "Usage: " << argv[0] << " [input file] [output file]";
		return EXIT_FAILURE;
	}

	DoProcess();

	// save output

	std::wofstream of(argv[2]);
	if (!of.good())
	{
		std::cerr << "error: cannot open output file" << std::endl;
		return EXIT_FAILURE;
	}

	of << output.rdbuf();
	return EXIT_SUCCESS;
}

void DoProcess()
{
	unsigned line = 1;

	std::wstring str;
	bool block = false;

	std::multimap<std::wstring, std::wstring> BlockMap;

	while (!input.eof())
	{
		getline(input, str);

		if (block)
		{
			if (str == L"#@endsort")
			{
				for (const auto &pr : BlockMap)
				{
					output << pr.second << '\n';
				}

				output << L"//#@endsort\n";
				block = false;
			}
			else
			{
				try
				{
					BlockMap.insert(std::make_pair(FindKeyString(str), str));
				}
				catch (std::runtime_error &e)
				{
					std::cerr << "line " << line << ": " << e.what() << std::endl;
					exit(EXIT_FAILURE);
				}
			}
		}
		else
		{
			if (str == L"#@sort")
			{
				output << L"//#@sort\n";
				block = true;
			}
			else
			{
				output << str << '\n';
			}
		}

		line++;
	}

	if (block)
	{
		std::cerr << "line " << line << ": unexpected end of file" << std::endl;
		exit(EXIT_FAILURE);
	}
}

std::wstring FindKeyString(const std::wstring &str)
{
	std::wstring::size_type start, end;

	start = str.find(L"\"");
	if (start == std::wstring::npos)
		throw std::runtime_error("there's no key string");

	end = str.find(L"\"", ++start);
	if (end == std::wstring::npos)
		throw std::runtime_error("key string must be placed in only one line");

	return str.substr(start, end);
}

