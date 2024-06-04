#ifndef TEST_WX_BITMAP_H
#define TEST_WX_BITMAP_H

#include <new>
#include <string>

constexpr int wxBITMAP_TYPE_PNG = 0;

class wxBitmap
{
public:
    inline static bool throwOnCopy = false;
    inline static int liveInstances = 0;

    wxBitmap() { ++liveInstances; }

    wxBitmap(const std::string &, int) { ++liveInstances; }

    wxBitmap(const wxBitmap &)
    {
        if (throwOnCopy)
            throw std::bad_alloc();
        ++liveInstances;
    }

    wxBitmap &operator=(const wxBitmap &)
    {
        if (throwOnCopy)
            throw std::bad_alloc();
        return *this;
    }

    ~wxBitmap() { --liveInstances; }
};

#endif
