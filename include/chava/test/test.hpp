#ifndef TEST
#define TEST

#include <expected>
#include <format>
#include <functional>
#include <string>
namespace Test {
    using test_result = std::expected<std::string, std::string>;
    using test_fn_ret = std::expected<void, std::vector<std::string>>;
    using test_fn = std::function<test_fn_ret()>;
    using assert_res = std::expected<void, std::string>;

    template<std::formattable<char> T>
    Test::assert_res assert_eq(const T& expected, const T& actual) {
        if(expected != actual) {
            return std::unexpected(std::format("Expected {}, got {}", expected, actual));
        }
        return {};
    }


    test_result run_test(test_fn test, const std::string& title);

    void collect_assert(std::vector<std::string>& results, const assert_res& res);
    Test::test_fn_ret get_result_ret(const std::vector<std::string>& results);

    std::string summarize_tests(const std::vector<test_result>& results);

}

// 2. Formatter for std::vector of that type
template <typename T>
struct std::formatter<std::vector<T>> : std::formatter<std::string> {
    template <typename FormatContext>
    auto format(const std::vector<T>& vec, FormatContext& ctx) const {
        std::string result = "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) result += ", ";
            // Format each element using std::format
            result += std::format("{}", vec[i]);
        }
        result += "]";
        return std::formatter<std::string>::format(result, ctx);
    }
};
#endif
