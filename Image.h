#pragma once
#include <vector>
#include <cstddef>
#include <algorithm>
#include <cmath>

class Image {
public:
    Image() : width_(0), height_(0) {}

    Image(size_t width, size_t height, float fillValue = 0.0f)
        : width_(width), height_(height), data_(width* height, fillValue) {}

    size_t width() const { return width_; }
    size_t height() const { return height_; }
    size_t size() const { return data_.size(); }

    float& operator()(size_t x, size_t y) {
        return data_[y * width_ + x];
    }

    const float& operator()(size_t x, size_t y) const {
        return data_[y * width_ + x];
    }

    float& at(size_t index) {
        return data_[index];
    }

    const float& at(size_t index) const {
        return data_[index];
    }

    const std::vector<float>& data() const { return data_; }
    std::vector<float>& data() { return data_; }

private:
    size_t width_;
    size_t height_;
    std::vector<float> data_;
};
