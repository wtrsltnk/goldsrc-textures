#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "texture.h"
#include "wadfile.h"

#include <map>
#include <vector>

class TextureManager
{
    std::map<std::string, Texture *> _textures;
    std::map<std::string, WadFile *> _wadfiles;

public:
    TextureManager();

    bool addTexture(Texture *texture);
    std::map<std::string, std::vector<Texture *>> findTextures(std::string const &searchFor);

    bool addTexturesFromWadFile(std::string const &filepath);
};

#endif // TEXTUREMANAGER_H
