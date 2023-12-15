#pragma once

#include <stdexcept>

class Exception : public std::exception {
    private:
        std::string message;
    public:
        Exception(std::string message) : message(std::move(message)) {}

        const char *what() const noexcept override {
            return message.c_str();
        }
};
