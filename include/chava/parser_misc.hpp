#ifndef PARSER_MISC_HPP
#define PARSER_MISC_HPP

#include <chava/common.hpp>
#include <variant>

template<typename V>
struct PositionWrapper {
    V value;
    Position pos;

    template<typename S>
    PositionWrapper<S> to() const {
        return PositionWrapper<S> {
            .value=std::get<S>(value),
            .pos=pos,
        };
    }
};

#endif
