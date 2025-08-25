#include "stdafx.h"

using namespace std;
using namespace Gdiplus;

void G00File0::FromImage(Bitmap* pImage)
{
    _width = pImage->GetWidth();
    _height = pImage->GetHeight();
    _pixels.resize(_width * _height);

    Rect imageRect(0, 0, _width, _height);
    BitmapData imageData;
    pImage->LockBits(&imageRect, ImageLockModeRead, PixelFormat24bppRGB, &imageData);

    for (int y = 0; y < _height; y++)
    {
        void* pFrom = (byte *)imageData.Scan0 + y*imageData.Stride;
        void* pTo = &_pixels[y * _width];
        memcpy(pTo, pFrom, _width*sizeof(Pixel));
    }

    pImage->UnlockBits(&imageData);
}

shared_ptr<Bitmap> G00File0::ToImage()
{
    shared_ptr<Bitmap> pImage = shared_ptr<Bitmap>(new Bitmap(_width, _height, PixelFormat24bppRGB));
    Rect imageRect(0, 0, _width, _height);
    BitmapData imageData;
    pImage->LockBits(&imageRect, ImageLockModeWrite, PixelFormat24bppRGB, &imageData);

    for (int y = 0; y < _height; y++)
    {
        void* pFrom = &_pixels[y * _width];
        void* pTo = (byte *)imageData.Scan0 + y*imageData.Stride;
        memcpy(pTo, pFrom, _width*sizeof(Pixel));
    }

    pImage->UnlockBits(&imageData);
    return pImage;
}

void G00File0::Read(Stream* pStream)
{
    ReadDimensions(pStream);
    _pixels.resize(_width * _height);

    int compressedSize = pStream->ReadInt32() - 8;
    int uncompressedSize = pStream->ReadInt32();
    assert(compressedSize == pStream->Size() - pStream->GetPosition());
    assert(uncompressedSize == _pixels.size()*4);
    shared_ptr<MemoryStream> pPixels = G00Compression::Decompress0(pStream, uncompressedSize);
    assert(pPixels->Size() == _pixels.size()*sizeof(Pixel));
    
    pPixels->ReadBytes(_pixels.data(), _pixels.size()*sizeof(Pixel));
}

void G00File0::Write(Stream* pStream)
{
    WriteDimensions(pStream);

    MemoryStream plain(_pixels.data(), _pixels.size()*sizeof(Pixel), true);
    shared_ptr<MemoryStream> pCompressed = G00Compression::Compress0(&plain);

    pStream->Write(8 + pCompressed->Size());
    pStream->Write(_pixels.size() * 4);
    pStream->Write(*pCompressed);
}
