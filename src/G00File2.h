#pragma once

class G00File2 : public G00File
{
private:
    struct Pixel
    {
        byte R;
        byte G;
        byte B;
        byte A;
    };

    struct Range
    {
        int Offset;
        int Size;
    };

    class Part
    {
    public:
        void            Read                (Stream* pStream);
        void            Write               (Stream* pStream);

        short X;
        short Y;
        short Width;
        short Height;
        std::vector<Pixel> Pixels;
    };

    class Region
    {
    public:
        int             GetWidth            () const;
        int             GetHeight           () const;

        void            ReadMetadata        (Stream* pStream);
        void            WriteMetadata       (Stream* pStream);

        void            ReadData            (Stream* pStream);
        void            WriteData           (Stream* pStream);

        int X1;
        int Y1;
        int X2;
        int Y2;
        int OriginX;
        int OriginY;
        std::vector<Part> Parts;
    };

    std::vector<Region> _regions;

public:
    virtual void                                FromImage               (Gdiplus::Bitmap* pImage) override;
    virtual std::shared_ptr<Gdiplus::Bitmap>    ToImage                 () override;

    std::shared_ptr<pugi::xml_document>         GetRegionXml            ();
    void                                        ApplyRegionXml          (pugi::xml_document* pDoc);

private:
    virtual void                                Read                    (Stream* pStream) override;
    virtual void                                Write                   (Stream* pStream) override;

    void                                        ReadRegionMetadatas     (Stream* pStream);
    void                                        WriteRegionMetadatas    (Stream* pStream);

    std::shared_ptr<MemoryStream>               ReadData                (Stream* pStream);
    void                                        WriteData               (Stream* pStream, Stream* pData);

    std::vector<Range>                          ReadRegionRanges        (Stream* pStream);
    void                                        WriteRegionRanges       (Stream* pStream, const std::vector<Range>& ranges);

    void                                        LayoutRegions           ();
};
