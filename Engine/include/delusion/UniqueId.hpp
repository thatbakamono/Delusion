#pragma once

#include "delusion/Random.hpp"

class UniqueId {
    private:
        uint64_t m_value;
    public:
        UniqueId() : m_value(Random::randomU64()) {}

        explicit UniqueId(uint64_t value) : m_value(value) {}

        [[nodiscard]] bool operator==(const UniqueId &other) const {
            return m_value == other.m_value;
        }

        [[nodiscard]] uint64_t value() const {
            return m_value;
        }
};

namespace std {
    template <>
    class hash<UniqueId> {
        public:
            std::size_t operator()(const UniqueId &uniqueId) const {
                return hash<uint64_t>()(uniqueId.value());
            }
    };
}
