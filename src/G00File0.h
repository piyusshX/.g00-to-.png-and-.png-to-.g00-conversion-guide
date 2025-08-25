#pragma once

class G00File0 : public G00File
{
private:
    struct Pixel
    {
        byte R;
        byte G;
        byte B;
    };

    std::vector<Pixel> _pixels;

public:
    virtual void                                FromImage           (Gdiplus::Bitmap* pImage) override;
    virtual std::shared_ptr<Gdiplus::Bitmap>    ToImage             () override;

private:
    virtual void                                Read                (Stream* pStream) override;
    virtual void                                Write               (Stream* pStream) override;
};
