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

    bool operator==(const PositionWrapper<V>& other) const {
        if constexpr (is_shared_ptr_v<V>) {
            return *value == *other.value && pos == other.pos;
        }
        return value == other.value && pos == other.pos;
    }
    bool operator!=(const PositionWrapper<V>& other) const {
        if constexpr (is_shared_ptr_v<V>) {
            return *value != *other.value || pos != other.pos;
        }
        return value != other.value || pos != other.pos;
    }
};

template<typename T>
bool variant_equal_ignore_shared_ptr(T& left, T& right) {
    return std::visit([](const auto& l, const auto& r) {
        using LT = std::decay_t<decltype(l)>;
        using RT = std::decay_t<decltype(r)>;
        if constexpr (std::is_same_v<LT, RT>) {
            if constexpr (is_shared_ptr_v<LT>) {
                return *l == *r;
            }
            return l == r;
        }

        return false;
    }, left, right);
}

#endif
