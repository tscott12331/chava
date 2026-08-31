#ifndef COMMON_HPP
#define COMMON_HPP

struct Position {
    int line;
    int col;
};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

#endif
