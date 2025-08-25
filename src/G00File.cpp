#include "stdafx.h"

using namespace std;

shared_ptr<G00File> G00File::Create(byte format)
{
    shared_ptr<G00File> pG00;
    switch (format)
    {
        case 0:
            pG00 = make_shared<G00File0>();
            break;

        case 2:
            pG00 = make_shared<G00File2>();
            break;

        default:
            throw exception("Unsupported g00 format");
    }
    pG00->_format = format;
    return pG00;
}

shared_ptr<G00File> G00File::Load(const wstring& filePath)
{
    shared_ptr<FileStream> pStream = File::Open(filePath);
    if (!pStream)
        throw exception("Input file does not exist");

    return Load(pStream.get());
}

shared_ptr<G00File> G00File::Load(Stream* pStream)
{
    byte format = pStream->ReadByte();
    shared_ptr<G00File> pG00 = Create(format);
    pG00->Read(pStream);
    return pG00;
}

void G00File::Save(const wstring& filePath)
{
    shared_ptr<FileStream> pStream = File::Create(filePath);
    Save(pStream.get());
}

void G00File::Save(Stream* pStream)
{
    pStream->Write(_format);
    Write(pStream);
}

void G00File::ReadDimensions(Stream* pStream)
{
    _width = pStream->ReadInt16();
    _height = pStream->ReadInt16();
}

void G00File::WriteDimensions(Stream* pStream)
{
    pStream->Write(_width);
    pStream->Write(_height);
}
