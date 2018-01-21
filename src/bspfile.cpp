#include "bspfile.h"
#include "texturemanager.h"
#include "wadtexture.h"
#include <fstream>

#define HL1_BSP_SIGNATURE 30
#define HL1_BSP_LUMPCOUNT 15

#define HL1_BSP_TEXTURELUMP 2

typedef struct sBSPLump
{
    int offset;
    int size;

} tBSPLump;

typedef struct sBSPHeader
{
    int signature;
    tBSPLump lumps[HL1_BSP_LUMPCOUNT];

} tBSPHeader;

typedef struct sBSPMipTexOffsetTable
{
    int miptexCount;
    int offsets[1]; /* an array with "miptexcount" number of offsets */

} tBSPMipTexOffsetTable;

typedef struct sBSPMipTexHeader
{
    char name[16];
    unsigned int width;
    unsigned int height;
    unsigned int offsets[4];

} tBSPMipTexHeader;

BspFile::BspFile()
{
}

bool BspFile::load(const std::string &filePath, TextureManager *textureManager)
{
    if (filePath == "")
    {
        return false;
    }

    if (textureManager == nullptr)
    {
        return false;
    }

    _filePath = filePath;

    std::ifstream stream;

    stream.open(filePath, std::ios::in | std::ios::binary);

    if (!stream.is_open())
    {
        return false;
    }

    tBSPHeader header;

    stream.read((char *)&header, sizeof(tBSPHeader));

    if (header.signature != HL1_BSP_SIGNATURE)
    {
        return false;
    }

    unsigned char *textureData = new unsigned char[header.lumps[HL1_BSP_TEXTURELUMP].size];

    stream.seekg(header.lumps[HL1_BSP_TEXTURELUMP].offset, std::ios_base::beg);
    stream.read((char *)textureData, header.lumps[HL1_BSP_TEXTURELUMP].size);

    int textureCount = int(*textureData);
    int *textureTable = (int *)(textureData + sizeof(int));

    for (int i = 0; i < textureCount; i++)
    {
        auto texture = textureData + textureTable[i];
        tBSPMipTexHeader *miptex = (tBSPMipTexHeader *)texture;

        if (miptex->offsets[0] == 0)
        {
            continue;
        }

        // And pass the data to the texture class who now owns the data and should cleanup
        auto t = new WadTexture(miptex->name, this, texture);
        textureManager->addTexture(t);
    }

    delete[] textureData;

    return true;
}
