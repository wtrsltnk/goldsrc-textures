#include "wadtexture.h"
#include "wadfile.h"

#include <glad/glad.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

typedef struct sBSPMipTexHeader
{
    char name[16];
    unsigned int width;
    unsigned int height;
    unsigned int offsets[4];

} tBSPMipTexHeader;

WadTexture::WadTexture(std::string const &name, TexturePackage *package, unsigned char *miptexData)
    : Texture(name, package), _pixels(getPixels(miptexData))
{
}

unsigned char *WadTexture::getPixels(unsigned char *miptexData)
{
    if (miptexData == nullptr)
    {
        return nullptr;
    }

    auto miptex = (tBSPMipTexHeader *)miptexData;

    if (miptex->height <= 0 || miptex->height > 1024 || miptex->width <= 0 || miptex->width > 1024)
    {
        return nullptr;
    }

    _width = miptex->width;
    _height = miptex->height;

    const int bpp = 4;
    int size = miptex->width * miptex->height;
    int paletteOffset = miptex->offsets[0] + size + (size / 4) + (size / 16) + (size / 64) + sizeof(short);

    // Get the miptex data and palette
    auto source0 = miptexData + miptex->offsets[0];
    auto palette = miptexData + paletteOffset;

    auto pixels = new unsigned char[size * bpp];

    unsigned char r, g, b, a;
    for (int i = 0; i < size; i++)
    {
        r = palette[source0[i] * 3];
        g = palette[source0[i] * 3 + 1];
        b = palette[source0[i] * 3 + 2];
        a = 255;

        // Do we need a transparent pixel
        if (miptex->name[0] == '{' && source0[i] == 255)
            r = g = b = a = 0;

        pixels[i * bpp + 0] = r;
        pixels[i * bpp + 1] = g;
        pixels[i * bpp + 2] = b;
        pixels[i * bpp + 3] = a;
    }

    return pixels;
}

bool WadTexture::writeToFile(std::string const &filename)
{
    if (_pixels == nullptr)
    {
        return false;
    }

    return stbi_write_bmp(filename.c_str(), width(), height(), 4, _pixels);
}

bool WadTexture::upload()
{
    if (_pixels == nullptr)
    {
        return false;
    }

    glGenTextures(1, &_glId);
    glBindTexture(GL_TEXTURE_2D, _glId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixels);

    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void WadTexture::cleanup()
{
    if (_pixels != nullptr)
    {
        delete[] _pixels;
        _pixels = nullptr;
    }

    if (_glId > 0)
    {
        glDeleteTextures(1, &_glId);
        _glId = 0;
    }
}
