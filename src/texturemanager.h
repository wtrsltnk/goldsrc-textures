#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "texture.h"
#include "wadfile.h"
#include "bspfile.h"

#include <map>
#include <vector>
#include <mutex>

class TextureManager
{
    std::map<std::string, Texture *> _textures;
    std::map<std::string, WadFile *> _wadfiles;
    std::map<std::string, BspFile *> _bspfiles;
    std::mutex _mutex;

public:
    TextureManager();

    bool addTexture(Texture *texture);
    std::map<std::string, std::vector<Texture *>> findTextures(std::string const &searchFor);

    bool addTexturesFromWadFile(std::string const &filepath);
    bool addTexturesFromBspFile(std::string const &filepath);

};

#endif // TEXTUREMANAGER_H
