﻿#include <fmt/core.h>
#include <string>
#include <charconv>
#include <array>

template <typename T>
std::string toChars(T Value) {
  std::array<char, 32> Buffer;
  auto b = &Buffer.front();
  auto e = &Buffer.back() + 1;
  return {b, std::to_chars(b, e, Value).ptr};
}

int main() {
  const double doublePi = 3.14159265358979;
  const float floatPi = doublePi;

  auto GrisuF = fmt::format("{}", floatPi);
  auto GrisuD = fmt::format("{}", doublePi);
  auto ToChrF = toChars(floatPi);
  auto ToChrD = toChars(doublePi);
}
