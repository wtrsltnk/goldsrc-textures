#include "texturemanager.h"
#include "texture.h"
#include "texturepackage.h"

#include <algorithm>
#include <cctype>
#include <string>

bool findStringIC(const std::string &strHaystack, const std::string &strNeedle)
{
    auto it = std::search(
        strHaystack.begin(), strHaystack.end(),
        strNeedle.begin(), strNeedle.end(),
        [](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); });
    return (it != strHaystack.end());
}

TextureManager::TextureManager()
{
}

bool TextureManager::addTexture(Texture *texture)
{
    if (texture == nullptr)
    {
        return false;
    }

    if (_textures.find(texture->name()) != _textures.end())
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(_mutex);

    _textures.insert(std::make_pair(texture->name(), texture));

    return true;
}

std::map<std::string, std::vector<Texture *>> TextureManager::findTextures(std::string const &searchFor)
{
    std::lock_guard<std::mutex> lock(_mutex);

    std::map<std::string, std::vector<Texture *>> result;

    for (auto pair : _textures)
    {
        if (searchFor != "" && !findStringIC(pair.first, searchFor))
        {
            continue;
        }

        if (result.find(pair.second->package()->filePath()) == result.end())
        {
            result.insert(std::make_pair(pair.second->package()->filePath(), std::vector<Texture *>()));
        }

        result[pair.second->package()->filePath()].push_back(pair.second);
    }

    return result;
}

bool TextureManager::addTexturesFromWadFile(std::string const &filepath)
{
    auto wadFile = new WadFile();

    if (!wadFile->load(filepath, this))
    {
        delete wadFile;
        return false;
    }

    std::lock_guard<std::mutex> lock(_mutex);

    _wadfiles.insert(std::make_pair(filepath, wadFile));

    return true;
}

bool TextureManager::addTexturesFromBspFile(std::string const &filepath)
{
    auto bspFile = new BspFile();

    if (!bspFile->load(filepath, this))
    {
        delete bspFile;
        return false;
    }

    std::lock_guard<std::mutex> lock(_mutex);

    _bspfiles.insert(std::make_pair(filepath, bspFile));

    return true;
}
