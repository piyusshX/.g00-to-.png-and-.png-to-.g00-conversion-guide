#include "stdafx.h"

using namespace std;
using namespace Gdiplus;
using namespace pugi;

void G00File2::FromImage(Bitmap* pImage)
{
    _width = pImage->GetWidth();
    _height = pImage->GetHeight();

    Rect imageRect(0, 0, _width, _height);
    BitmapData imageData;
    pImage->LockBits(&imageRect, ImageLockModeRead, PixelFormat32bppARGB, &imageData);

    for (Region& region : _regions)
    {
        region.Parts.clear();
        if (region.GetWidth() <= 1 || region.GetHeight() <= 1)
            continue;

        region.Parts.push_back(Part());
        Part& part = *(region.Parts.end() - 1);

        part.X = 0;
        part.Y = 0;
        part.Width = region.GetWidth();
        part.Height = region.GetHeight();
        part.Pixels.resize(part.Width * part.Height);

        int xBase = region.X1;
        int yBase = region.Y1;
        for (int yOffset = 0; yOffset < part.Height; yOffset++)
        {
            void* pFrom = (byte *)imageData.Scan0 + (yBase + yOffset)*imageData.Stride + xBase*sizeof(Pixel);
            void* pTo = &part.Pixels[yOffset * part.Width];
            memcpy(pTo, pFrom, part.Width*sizeof(Pixel));
        }
    }

    pImage->UnlockBits(&imageData);
}

shared_ptr<Bitmap> G00File2::ToImage()
{
    shared_ptr<Bitmap> pImage(new Bitmap(_width, _height, PixelFormat32bppARGB));
    Rect imageRect(0, 0, _width, _height);
    BitmapData imageData;
    pImage->LockBits(&imageRect, ImageLockModeWrite, PixelFormat32bppARGB, &imageData);

    for (Region& region : _regions)
    {
        for (Part& part : region.Parts)
        {
            int xBase = region.X1 + part.X;
            int yBase = region.Y1 + part.Y;
            for (int yOffset = 0; yOffset < part.Height; yOffset++)
            {
                void* pFrom = &part.Pixels[yOffset * part.Width];
                void* pTo = (byte *)imageData.Scan0 + (yBase + yOffset)*imageData.Stride + xBase*sizeof(Pixel);
                memcpy(pTo, pFrom, part.Width*sizeof(Pixel));
            }
        }
    }

    pImage->UnlockBits(&imageData);
    return pImage;
}

shared_ptr<xml_document> G00File2::GetRegionXml()
{
    shared_ptr<xml_document> pDoc = make_shared<xml_document>();

    xml_node rootElem = pDoc->append_child(L"vas_g00");
    rootElem.append_attribute(L"format").set_value(_format);

    xml_node regionsElem = rootElem.append_child(L"regions");
    for (Region& region : _regions)
    {
        xml_node regionElem = regionsElem.append_child(L"region");
        regionElem.append_attribute(L"x1").set_value(region.X1);
        regionElem.append_attribute(L"y1").set_value(region.Y1);
        regionElem.append_attribute(L"x2").set_value(region.X2);
        regionElem.append_attribute(L"y2").set_value(region.Y2);
        if (region.OriginX != 0 || region.OriginY != 0)
        {
            xml_node originElem = regionElem.append_child(L"origin");
            originElem.append_attribute(L"x").set_value(region.OriginX);
            originElem.append_attribute(L"y").set_value(region.OriginY);
        }
    }

    return pDoc;
}

void G00File2::ApplyRegionXml(xml_document* pDoc)
{
    _regions.clear();

    xml_node rootElem = pDoc->document_element();
    if (wcscmp(rootElem.name(), L"vas_g00") != 0)
        throw exception("Invalid XML root element name");

    xml_node regionsElem = rootElem.child(L"regions");
    if (!regionsElem)
        throw exception("<regions> element not found");

    for (xml_node& regionElem : regionsElem.children(L"region"))
    {
        Region region;
        region.X1 = regionElem.attribute(L"x1").as_int();
        region.Y1 = regionElem.attribute(L"y1").as_int();
        region.X2 = regionElem.attribute(L"x2").as_int();
        region.Y2 = regionElem.attribute(L"y2").as_int();

        xml_node originElem = regionElem.child(L"origin");
        if (originElem)
        {
            region.OriginX = originElem.attribute(L"x").as_int();
            region.OriginY = originElem.attribute(L"y").as_int();
        }
        else
        {
            region.OriginX = 0;
            region.OriginY = 0;
        }

        _regions.push_back(region);
    }
}

void G00File2::Read(Stream* pStream)
{
    ReadDimensions(pStream);
    ReadRegionMetadatas(pStream);
    shared_ptr<MemoryStream> pDataStream = ReadData(pStream);
    vector<Range> regionRanges = ReadRegionRanges(pDataStream.get());
    assert(regionRanges.size() == _regions.size());

    for (int i = 0; i < _regions.size(); i++)
    {
        Region& region = _regions[i];
        Range& range = regionRanges[i];
        if (range.Size <= 0)
            continue;

        assert(range.Offset >= 0);
        assert(range.Offset + range.Size <= pDataStream->Size());
        MemoryStream regionStream((byte *)pDataStream->Data() + range.Offset, range.Size, true);
        region.ReadData(&regionStream);
    }

    LayoutRegions();
}

void G00File2::Write(Stream* pStream)
{
    WriteDimensions(pStream);
    WriteRegionMetadatas(pStream);

    vector<Range> regionRanges(_regions.size());
    MemoryStream dataStream;
    WriteRegionRanges(&dataStream, regionRanges);
    for (int i = 0; i < _regions.size(); i++)
    {
        Region& region = _regions[i];
        Range& range = regionRanges[i];

        range.Offset = dataStream.GetPosition();
        if (region.GetWidth() > 1 && region.GetHeight() > 1)
            region.WriteData(&dataStream);
        
        range.Size = dataStream.GetPosition() - range.Offset;
    }

    dataStream.SetPosition(0);
    WriteRegionRanges(&dataStream, regionRanges);

    dataStream.SetPosition(0);
    WriteData(pStream, &dataStream);
}

void G00File2::ReadRegionMetadatas(Stream* pStream)
{
    int numRegions = pStream->ReadInt32();
    _regions.resize(numRegions);
    for (int i = 0; i < numRegions; i++)
    {
        _regions[i].ReadMetadata(pStream);
    }
}

void G00File2::WriteRegionMetadatas(Stream* pStream)
{
    pStream->Write(_regions.size());
    for (Region& region : _regions)
    {
        region.WriteMetadata(pStream);
    }
}

shared_ptr<MemoryStream> G00File2::ReadData(Stream* pStream)
{
    int compressedSize = pStream->ReadInt32() - 8;
    int uncompressedSize = pStream->ReadInt32();
    assert(compressedSize == pStream->Size() - pStream->GetPosition());
    return G00Compression::Decompress1(pStream, uncompressedSize);
}

void G00File2::WriteData(Stream* pStream, Stream* pData)
{
    shared_ptr<MemoryStream> pCompressed = G00Compression::Compress1(pData);
    pStream->Write(8 + pCompressed->Size());
    pStream->Write(pData->Size());
    pStream->Write(*pCompressed.get());
}

vector<G00File2::Range> G00File2::ReadRegionRanges(Stream* pStream)
{
    int numRegions = pStream->ReadInt32();
    vector<Range> ranges(numRegions);
    for (int i = 0; i < numRegions; i++)
    {
        ranges[i].Offset = pStream->ReadInt32();
        ranges[i].Size = pStream->ReadInt32();
    }
    return ranges;
}

void G00File2::WriteRegionRanges(Stream* pStream, const vector<Range>& ranges)
{
    pStream->Write(ranges.size());
    for (const Range& range : ranges)
    {
        pStream->Write(range.Offset);
        pStream->Write(range.Size);
    }
}

void G00File2::LayoutRegions()
{
    _width = 0;
    _height = 0;
    for (Region& region : _regions)
    {
        int regionWidth = region.GetWidth();
        int regionHeight = region.GetHeight();
        region.X1 = 0;
        region.X2 = region.X1 + regionWidth - 1;
        region.Y1 = _height;
        region.Y2 = region.Y1 + regionHeight - 1;
        _width = max((int)_width, regionWidth);
        _height += regionHeight;
    }
}

int G00File2::Region::GetWidth() const
{
    return X2 - X1 + 1;
}

int G00File2::Region::GetHeight() const
{
    return Y2 - Y1 + 1;
}

void G00File2::Region::ReadMetadata(Stream* pStream)
{
    X1 = pStream->ReadInt32();
    Y1 = pStream->ReadInt32();
    X2 = pStream->ReadInt32();
    Y2 = pStream->ReadInt32();
    OriginX = pStream->ReadInt32();
    OriginY = pStream->ReadInt32();
}

void G00File2::Region::WriteMetadata(Stream* pStream)
{
    pStream->Write(X1);
    pStream->Write(Y1);
    pStream->Write(X2);
    pStream->Write(Y2);
    pStream->Write(OriginX);
    pStream->Write(OriginY);
}

void G00File2::Region::ReadData(Stream* pStream)
{
    short type = pStream->ReadInt16();
    assert(type == 1);

    int numParts = pStream->ReadUInt16();
    pStream->Seek(0x20 + 0x50);
    Parts.resize(numParts);
    for (int i = 0; i < numParts; i++)
    {
        Parts[i].Read(pStream);
    }
}

void G00File2::Region::WriteData(Stream* pStream)
{
    pStream->Write((short)1);
    pStream->Write((short)Parts.size());
    pStream->Write(0);
    pStream->Write(0);
    pStream->Write(GetWidth());
    pStream->Write(GetHeight());
    pStream->Write(OriginX);
    pStream->Write(OriginY);
    pStream->Write(GetWidth());
    pStream->Write(GetHeight());

    for (int i = 0; i < 0x50; i++)
    {
        pStream->Write((byte)0);
    }

    for (Part& part : Parts)
    {
        part.Write(pStream);
    }
}

void G00File2::Part::Read(Stream* pStream)
{
    X = pStream->ReadInt16();
    Y = pStream->ReadInt16();
    short trans = pStream->ReadInt16();
    Width = pStream->ReadInt16();
    Height = pStream->ReadInt16();
    short padding = pStream->ReadInt16();
    pStream->Seek(0x50);

    Pixels.resize(Width * Height);
    pStream->ReadBytes(Pixels.data(), sizeof(Pixel) * Pixels.size());
}

void G00File2::Part::Write(Stream* pStream)
{
    pStream->Write(X);
    pStream->Write(Y);
    pStream->Write((short)1);
    pStream->Write(Width);
    pStream->Write(Height);
    pStream->Write((short)0);
    for (int i = 0; i < 0x50; i++)
    {
        pStream->Write((byte)0);
    }

    pStream->Write(Pixels.data(), sizeof(Pixel) * Pixels.size());
}
