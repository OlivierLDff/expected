#include <tl/expected.hpp>

#include <iostream>
#include <string>
#include <system_error>

using DivisionResult = tl::expected<int, std::error_code>;

auto do_division(int a, int b) -> DivisionResult {
  if (b == 0)
    return tl::make_unexpected(
        std::make_error_code(std::errc::invalid_argument));
  return a / b;
}

void print_result_division(int a, int b) {
  do_division(a, b)
      .and_then([&](int result) -> DivisionResult {
        std::cout << std::to_string(a) << "/" << std::to_string(b) << "="
                  << std::to_string(result) << std::endl;
        return result;
      })
      .or_else([&](std::error_code ec) -> DivisionResult {
        std::cout << "Can't divide " << a << " with " << b << ": "
                  << ec.message() << std::endl;
        return tl::make_unexpected(ec);
      });
}

int main() {
  print_result_division(10, 5);
  print_result_division(2, 0);
  return 0;
}
