#pragma once

template<typename T>
T max(T left, T right) {
    if (left > right) {
        return left;
    } else {
        return right;
    }
}

template<typename T>
T min(T left, T right) {
    if (left < right) {
        return left;
    } else {
        return right;
    }
}
