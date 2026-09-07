#ifndef COMMON_HPP
#define COMMON_HPP

struct Position {
    int line;
    int col;

    bool operator==(const Position& other) const {
        return line == other.line && col == other.col;
    }
};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

#endif
