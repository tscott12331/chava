#ifndef COMMON_HPP
#define COMMON_HPP

#include <memory>
#include <type_traits>

struct Position {
    int line;
    int col;

    bool operator==(const Position& other) const {
        return line == other.line && col == other.col;
    }
};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

template<typename T>
struct is_shared_ptr : std::false_type {};

template<typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

template<typename T>
inline constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;

#endif
