#pragma once

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace test {

using TestFunction = std::function<void()>;

struct Case {
    std::string name;
    TestFunction fn;
};

inline std::vector<Case>& registry() {
    static std::vector<Case> cases;
    return cases;
}

struct Registrar {
    Registrar(const std::string& name, TestFunction fn) { registry().push_back({name, std::move(fn)}); }
};

inline void require(bool condition, std::string_view message) {
    if (!condition) {
        throw std::runtime_error(std::string(message));
    }
}

inline int runAll() {
    int failures = 0;
    for (const auto& testCase : registry()) {
        try {
            testCase.fn();
            std::cout << "[PASS] " << testCase.name << '\n';
        } catch (const std::exception& ex) {
            ++failures;
            std::cerr << "[FAIL] " << testCase.name << ": " << ex.what() << '\n';
        }
    }
    return failures == 0 ? 0 : 1;
}

} // namespace test

#define TEST_CASE(name) \
    void name();         \
    static test::Registrar registrar_##name(#name, name); \
    void name()
