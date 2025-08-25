#include "stdafx.h"

using namespace std;
using namespace Gdiplus;

Gdi::Gdi()
{
    GdiplusStartupInput options;
    GdiplusStartup(&_pToken, &options, nullptr);
}

Gdi::~Gdi()
{
    GdiplusShutdown(_pToken);
}

void Gdi::SaveImage(Image* pImage, const wstring& filePath)
{
    wstring extension = Path::GetExtension(filePath);
    if (extension.empty())
        throw exception("Could not determine file extension");

    wstring mimeType = L"image/" + extension;
    CLSID encoderClsId = GetEncoderClsId(mimeType);
    pImage->Save(filePath.c_str(), &encoderClsId);
}

CLSID Gdi::GetEncoderClsId(const wstring& mimeType)
{
    uint numEncoders;
    uint encodersArraySize;
    GetImageEncodersSize(&numEncoders, &encodersArraySize);

    vector<byte> encoders(encodersArraySize);
    ImageCodecInfo* pEncoders = (ImageCodecInfo *)encoders.data();
    GetImageEncoders(numEncoders, encodersArraySize, pEncoders);
    for (int i = 0; i < numEncoders; i++)
    {
        if (pEncoders[i].MimeType == mimeType)
            return pEncoders[i].Clsid;
    }
    throw exception("Encoder not found");
}
