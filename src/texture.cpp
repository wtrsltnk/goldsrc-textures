#include "texture.h"
#include "texturepackage.h"

Texture::Texture(std::string const &name, TexturePackage *package)
    : _name(name), _package(package), _width(0), _height(0), _glId(0)
{}

Texture::~Texture()
{
    cleanup();
}

std::string const &Texture::name() const
{
    return _name;
}

TexturePackage const *Texture::package() const
{
    return _package;
}

int Texture::width() const
{
    return _width;
}

int Texture::height() const
{
    return _height;
}

unsigned int Texture::glId() const
{
    return _glId;
}
