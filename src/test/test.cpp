#include <chava/test/test.hpp>
#include <cstddef>
#include <format>

Test::test_result Test::run_test(test_fn test, const std::string& title) {
    const auto& res = test();
    if(!res) {
        const auto failed_title = std::format("{}: FAILED", title);
        if(res.error().size() == 0) {
            return std::unexpected(failed_title);
        }
        std::size_t size = 0;
        for(const auto& s : res.error()) size += s.size();
        std::string err_strs{};
        err_strs.reserve(size);
        for(const auto& err : res.error()) err_strs += "\n\t" + err;
        return std::unexpected(std::format("{}{}", failed_title, err_strs));
    }

    return std::format("{}: PASSED", title);
}

void Test::collect_assert(std::vector<std::string>& results, const Test::assert_res& res) {
    if(!res) results.push_back(res.error());
}

std::string Test::summarize_tests(const std::vector<Test::test_result>& results) {
    auto total = 0;
    auto passed = 0;
    
    std::size_t results_size = 0;
    for(const auto& res : results) {
        if(res) {
            results_size += res.value().size();
            passed += 1;
        } else {
            results_size += res.error().size();
        }

        total += 1;
    }

    std::string joined_results{};
    joined_results.reserve(results_size);

    if(results.size() > 0) {
        const auto& res = results.at(0);
        if(res) {
            joined_results += res.value();
        } else {
            joined_results += res.error();
        }
    }

    for(const auto& res : std::span(results).subspan(1)) {
        if(res) {
            joined_results += "\n\n" + res.value();
        } else {
            joined_results += "\n\n" + res.error();
        }
    }

    return std::format("------ RESULTS ------\n\n{}\n\n{} passed, {} failed, {} total\n", joined_results, passed, total-passed, total);
}

Test::test_fn_ret Test::get_result_ret(const std::vector<std::string>& results) {
    if(results.size() > 0) return std::unexpected(results);
    return {};
}
