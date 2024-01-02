#pragma once

#include <mutex>

#include "delusion/collections/Queue.hpp"

class AudioBuffer {
    public:
        std::mutex mutex;

        Queue<float> samples;

        size_t currentSampleIndex{};
        size_t decodedSamples{};

        bool finishedDecoding{};

        explicit AudioBuffer(size_t capacity) : samples(capacity) {}

        AudioBuffer(const AudioBuffer &) = delete;

        AudioBuffer(AudioBuffer &&) noexcept = delete;

        AudioBuffer &operator=(const AudioBuffer &) = delete;

        AudioBuffer &operator=(AudioBuffer &&) noexcept = delete;
};
