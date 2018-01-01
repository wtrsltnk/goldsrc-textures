#ifndef WADTEXTURE_H
#define WADTEXTURE_H

#include "texture.h"

class WadTexture : public Texture
{
    unsigned char *_miptexData;

public:
    WadTexture(std::string const &name, class TexturePackage *package, unsigned char *miptexData);

    virtual bool upload();
    virtual void cleanup();
};

#endif // WADTEXTURE_H
