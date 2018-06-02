#define _CRT_SECURE_NO_DEPRECATE
#include <stdafx.h>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include "fmt/printf.h"

#include <cassert>

#include <iostream>
#include <support.h>

int main() {
	setupConsole();

	const auto t = std::time(nullptr);
	// Prints "The date is 2016-04-29." (with the current date)
	auto sResult = fmt::format("The date is {:%Y-%B-%d}.", fmt::localtime(t));
	auto wResult = fmt::format(L"The date is {:%Y-%B-%d}.", fmt::localtime(t));
	fmt::print("The date is {:%Y-%B-%d %A}.\n", fmt::localtime(t));

	sResult = fmt::sprintf("Elapsed time: %.2f seconds", 1.23);
	wResult = fmt::sprintf(L"Elapsed time: %.2f seconds", 1.23);
	fmt::print(std::cout, "Elapsed time: {:.2f} {}\n", 1.23, "seconds");
	fmt::print(std::wcout, L"Elapsed time: {:.2f} {}\n", 1.23, L"seconds");
	std::string sTo;
	fmt::format_to(std::back_inserter(sTo), "Elapsed time: {:.2f} seconds", 1.23);
	assert(sResult == sTo);
	std::wstring wTo;
	fmt::format_to(std::back_inserter(wTo), L"Elapsed time: {:.2f} seconds", 1.23);
	assert(sResult == sTo);

	fmt::print("Elapsed time: {:.2f} {}\n", 1.23, "seconds");
	fmt::print(fmt::fg(fmt::color::green), "Elapsed time: {:.2f} {}{}\n", 1.23, 'm', "seconds");
	fmt::print(fmt::fg(fmt::color::green) | fmt::bg(fmt::color::antique_white), "Elapsed time: {:.2f} {}{}\n", 1.23, 'm', "seconds");
	fmt::print(stdout, "Elapsed time: {:.2f} {}\n", 1.23, "seconds");
	auto n = fmt::printf("Elapsed time: %.2f %cseconds\n", 1.23, 'm');
	n = fmt::fprintf(stdout, "Elapsed time: %.2f %cseconds\n", 1.23, 'm');

	switchToWide(stdout);

	fmt::print(L"The date is {:%Y-%B-%d %A}.\n", fmt::localtime(t));
	fmt::print(L"Elapsed time: {:.2f} {}\n", 1.23, L"seconds");
	fmt::print(fmt::fg(fmt::color::green), L"Elapsed time: {:.2f} {}{}\n", 1.23, L'm', L"seconds");
	fmt::print(fmt::fg(fmt::color::green) | fmt::bg(fmt::color::blanched_almond), L"Elapsed time: {:.2f} {}{}\n", 1.23, L'm', L"seconds");
	fmt::print(stdout, L"Elapsed time: {:.2f} {}\n", 1.23, L"seconds");
	n = fmt::printf(L"Elapsed time: %.2f %cseconds\n", 1.23, L'm');
	n = fmt::fprintf(stdout, L"Elapsed time: %.2f %cseconds\n", 1.23, L'm');

	UNUSED(n);
	UNUSED(sResult);
	UNUSED(wResult);
}

