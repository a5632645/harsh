#pragma once

namespace mana {
class ImageBase {
public:
    struct SimplePixel {
        int r;
        int g;
        int b;
    };

    virtual ~ImageBase() = default;

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual SimplePixel GetPixel(int x, int y) const = 0;
};
}