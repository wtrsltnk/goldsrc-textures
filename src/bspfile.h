#ifndef BSPFILE_H
#define BSPFILE_H

#include "texturepackage.h"

class BspFile : public TexturePackage
{
public:
    BspFile();

    virtual bool load(std::string const &filePath, class TextureManager *textureManager);
};

#endif // BSPFILE_H
