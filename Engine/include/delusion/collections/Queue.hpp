#pragma once

#include <memory>
#include <stdexcept>

template <typename T>
class Queue {
    private:
        std::unique_ptr<T[]> elements {};
        size_t m_head {};
        size_t m_size {};
        size_t m_capacity {};
    public:
        explicit Queue(size_t capacity) : m_capacity(capacity) {
            if (capacity == 0) {
                throw std::invalid_argument("Capacity must be greater than 0");
            }

            elements = std::make_unique<T[]>(capacity);
        }

        Queue(const Queue &) = delete;

        Queue(Queue &&) noexcept = delete;

        Queue &operator=(const Queue &) = delete;

        Queue &operator=(Queue &&) noexcept = delete;

        void push(T element) {
            if (m_size >= m_capacity) {
                throw std::out_of_range("Queue is full");
            }

            elements[(m_head + m_size) % m_capacity] = element;
            m_size += 1;
        }

        [[nodiscard]] T &front() {
            return elements[m_head];
        }

        void pop() {
            if (m_size == 0) {
                throw std::out_of_range("Queue is empty");
            }

            m_head = (m_head + 1) % m_capacity;
            m_size -= 1;
        }

        [[nodiscard]] size_t size() const {
            return m_size;
        }

        [[nodiscard]] size_t capacity() const {
            return m_capacity;
        }
};
