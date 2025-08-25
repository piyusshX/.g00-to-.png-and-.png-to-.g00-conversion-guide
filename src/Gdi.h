#pragma once

class Gdi
{
private:
    ULONG_PTR _pToken;

public:
                    Gdi                     ();
                    ~Gdi                    ();

    void            SaveImage               (Gdiplus::Image* pImage, const std::wstring& filePath);

private:
    static CLSID    GetEncoderClsId         (const std::wstring& mimeType);
};
