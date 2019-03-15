import fmt;

#include <iterator>

struct implicitly_convertible_to_string_view {
  operator fmt::string_view() const { return "foo"; }
};

int main() {
  auto result = fmt::format("{}", 42);
  fmt::format_to(std::back_inserter(result), "{}", 42);
  result = fmt::format("{}", implicitly_convertible_to_string_view());
}
