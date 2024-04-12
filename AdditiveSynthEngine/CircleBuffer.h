#pragma once
#include <vector>

template<class T>
class CircleBuffer {
public:
    using Type = T;

    CircleBuffer(size_t size) {
        m_head = 0;
        audio_buffer_.resize(size);
    }

    template<class TT>
    void push(TT&& value) {
        audio_buffer_[m_head] = std::forward<TT>(value);
        m_head++;
        if (m_head == audio_buffer_.size()) {
            m_head = 0;
        }
    }

    size_t getSize() const {
        return audio_buffer_.size();
    }

    size_t getLastWriteIndex() const {
        return (m_head - 1 + getSize()) % getSize();
    }

    size_t getLastItemIndex() const {
        return m_head % getSize();
    }

    T& operator[](size_t index) {
        auto i = index % getSize();
        return audio_buffer_[i];
    }

    T const& operator[](size_t index) const {
        return this->operator[](index);
    }
private:
    size_t m_head;
    std::vector<Type> audio_buffer_;
};