/******************************************************************************
Copyright (c) 2020 Matthew Roever

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

// STL
#include <chrono>
#include <iostream>

// Windows
#include <Windows.h>

// Unchecked division. Used to measure function call + division overhead.
int unsafe_div(int numerator, int denominator) {
	return numerator / denominator;
}

// Check if denominator is zero before dividing.
int checked_div(int numerator, int denominator) {
	if (denominator == 0) throw std::runtime_error("Divide by zero. -checked_div");
	else return numerator / denominator;
}

// Catch SEH exception if divide by zero happens.
int seh_div(int numerator, int denominator) {
	__try {
		return numerator / denominator;
	}
	__except (GetExceptionCode() == EXCEPTION_INT_DIVIDE_BY_ZERO ?
		EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
		throw std::runtime_error("Divide by zero. -seh_div");
	}
}

// Light-weight benchmarking function.
using pDiv = int(int, int);
void benchmark(pDiv* divFunc, const char* method) {
	auto startTime = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 1'000'000'000; ++i) {
		divFunc(i, 2);
	}
	auto stopTime = std::chrono::high_resolution_clock::now();
	std::cout << "Method: " << method << "\nBenchmark time (s): " <<
		((double)std::chrono::duration_cast<std::chrono::microseconds>(
			stopTime - startTime).count() / 1.0E6) << "\n";
}

int main() {
	// Checked division
	try {
		std::cout << checked_div(42, 0) << "\n";
	}
	catch (const std::exception& err) {
		std::cout << err.what() << "\n";
	}

	// SEH division
	try {
		std::cout << seh_div(42, 0) << "\n";
	}
	catch (const std::exception& err) {
		std::cout << err.what() << "\n";
	}

	// Benchmark the methods - first pass times are inconsistent
	benchmark(unsafe_div, "unsafe_div");
	benchmark(checked_div, "checked_div");
	benchmark(seh_div, "seh_div");

	// Second pass has consistent times
	benchmark(unsafe_div, "unsafe_div");
	benchmark(checked_div, "checked_div");
	benchmark(seh_div, "seh_div");

	// MSVC =)
	system("pause");
	return 0;
}