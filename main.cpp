#include <iomanip>
#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>
using namespace std::literals;

template <class T> struct always_false : std::false_type
{};

using var_t = std::variant<int, long, double, std::string>;

// 所有Ts::operator()集合
template <class... Ts> struct overloaded : Ts...
{
  using Ts::operator()...;
};

// 根据 overloaded 构造器参数的类型来推导 overloaded 的模板参数类型
template <class... Ts> overloaded(Ts...)->overloaded<Ts...>;

int main()
{
  // #01 @variant_size
  std::variant<double, int, std::string, float> v("abc");
  std::cout << "size = " << std::variant_size<decltype(v)>::value << '\n';

  // #02 @index @get
  std::cout << "[" << v.index() << "]=" << std::get<std::string>(v) << '\n';

  // #03 @get_if
  if (auto pval = std::get_if<int>(&v)) {
    std::cout << " variant value " << *pval << '\n';
  } else {
    std::cout << "failed to get value\n";
  }

  // #04 @bad_variant_access
  try {
    float f = std::get<float>(v);
  } catch (const std::bad_variant_access& e) {
    std::cout << e.what() << '\n';
  }

  // #05 @holds_alternative
  std::variant<std::string> x("abc");  // 最优匹配
  std::variant<std::string, char const*> y("abc");
  if (std::holds_alternative<char const*>(y)) {
    std::cout << "[" << y.index() << "]=" << std::get<char const*>(y) << '\n';
  }

  // #06 @visit @quoted
  std::vector<var_t> vec = {10, 15l, 1.5, "hello"};
  for (auto&& v : vec) {
    std::visit([](auto&& arg) { std::cout << arg << '\t'; }, v);
    var_t w = std::visit([](auto&& arg) -> var_t { return arg + arg; }, v);
    std::cout << " after doubling ";
    std::visit(
        [](auto&& arg) {
          using T = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<T, int>) {
            std::cout << "int    with value " << arg << '\n';
          } else if constexpr (std::is_same_v<T, long>) {
            std::cout << "long   with value " << arg << '\n';
          } else if constexpr (std::is_same_v<T, double>) {
            std::cout << "double with value " << arg << '\n';
          } else if constexpr (std::is_same_v<T, std::string>) {
            std::cout << "string with value " << std::quoted(arg) << '\n';
          } else {
            static_assert(always_false<T>::value, "non-exhaustive visitor!");
          }
        },
        w);
  }

  for (auto&& v : vec) {
    std::visit(
        overloaded{
            [](auto arg) { std::cout << arg << ' '; },
            [](double arg) { std::cout << std::fixed << arg << ' '; },
            [](const std::string& arg) { std::cout << std::quoted(arg) << ' '; },
        },
        v);
  }
}