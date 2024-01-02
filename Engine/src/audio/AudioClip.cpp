#include "delusion/audio/AudioClip.hpp"

AudioClip::~AudioClip() {
    avcodec_free_context(&codecContext);
    avformat_close_input(&m_formatContext);
    av_freep(&m_avioContext->buffer);
    avio_context_free(&m_avioContext);
}

std::shared_ptr<AudioClip> AudioClip::create(const std::filesystem::path &path) {
    return create(UniqueId(), path);
}

std::shared_ptr<AudioClip> AudioClip::create(UniqueId id, const std::filesystem::path &path) {
    std::vector<uint8_t> bytes;

    {
        std::ifstream fileStream(path, std::ios::binary);

        fileStream.seekg(0, std::ios::end);

        bytes.reserve(fileStream.tellg());

        fileStream.seekg(0, std::ios::beg);

        bytes.assign(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
    }

    AVFormatContext *formatContext = avformat_alloc_context();

    if (formatContext == nullptr) {
        throw std::exception();
    }

    auto size = bytes.size();
    auto *data = static_cast<unsigned char *>(av_malloc(size));

    std::memcpy(data, bytes.data(), size);

    AVIOContext *avioContext = avio_alloc_context(data, size, 0, nullptr, nullptr, nullptr, nullptr);

    formatContext->pb = avioContext;
    formatContext->flags |= AVFMT_FLAG_CUSTOM_IO;

    if (avformat_open_input(&formatContext, nullptr, nullptr, nullptr) < 0) {
        throw std::exception();
    }

    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        throw std::exception();
    }

    int audioStreamIndex = -1;

    for (int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndex = i;

            break;
        }
    }

    if (audioStreamIndex == -1) {
        throw std::exception("No audio stream found");
    }

    const AVCodec *codec = avcodec_find_decoder(formatContext->streams[audioStreamIndex]->codecpar->codec_id);

    if (codec == nullptr) {
        throw std::exception();
    }

    AVCodecContext *codecContext = avcodec_alloc_context3(codec);

    if (codecContext == nullptr) {
        throw std::exception();
    }

    if (avcodec_parameters_to_context(codecContext, formatContext->streams[audioStreamIndex]->codecpar) < 0) {
        throw std::exception();
    }

    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        throw std::exception();
    }

    return std::shared_ptr<AudioClip>(new AudioClip(id, bytes, formatContext, avioContext, codecContext));
}
