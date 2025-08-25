#pragma once

class G00File
{
protected:
    byte _format;
    short _width;
    short _height;

public:
    static std::shared_ptr<G00File>             Create                  (byte format);

    static std::shared_ptr<G00File>             Load                    (const std::wstring& filePath);
    static std::shared_ptr<G00File>             Load                    (Stream* pStream);

    void                                        Save                    (const std::wstring& filePath);
    void                                        Save                    (Stream* pStream);

    virtual void                                FromImage               (Gdiplus::Bitmap* pImage) = 0;
    virtual std::shared_ptr<Gdiplus::Bitmap>    ToImage                 () = 0;

protected:
    virtual void                                Read                    (Stream* pStream) = 0;
    virtual void                                Write                   (Stream* pStream) = 0;

    void                                        ReadDimensions          (Stream* pStream);
    void                                        WriteDimensions         (Stream* pStream);
};
