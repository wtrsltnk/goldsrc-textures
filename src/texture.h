#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

class Texture
{
protected:
    std::string _name;
    class TexturePackage *_package;
    int _width;
    int _height;
    unsigned int _glId;

public:
    Texture(std::string const &name, class TexturePackage *package);
    virtual ~Texture();

    virtual bool writeToFile(std::string const &filename) { return false; }
    virtual bool upload() { return false; }
    virtual void cleanup() {}

    std::string const &name() const;
    class TexturePackage const *package() const;
    int width() const;
    int height() const;
    unsigned int glId() const;
};

#endif // TEXTURE_H
