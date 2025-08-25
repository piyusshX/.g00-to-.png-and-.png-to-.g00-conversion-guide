#include "stdafx.h"

using namespace std;

FileStream::FileStream(FILE* pFile)
{
    _pFile = pFile;
}

FileStream::~FileStream()
{
    fclose(_pFile);
    _pFile = nullptr;
}

int FileStream::GetPosition() const
{
    return ftell(_pFile);
}

void FileStream::SetPosition(int position)
{
    fseek(_pFile, position, SEEK_SET);
}

void FileStream::Seek(int offset)
{
    fseek(_pFile, offset, SEEK_CUR);
}

int FileStream::Size() const
{
    int position = ftell(_pFile);
    fseek(_pFile, 0, SEEK_END);
    int size = ftell(_pFile);
    fseek(_pFile, position, SEEK_SET);
    return size;
}

void FileStream::SkipBits(int count)
{
    throw exception("Not implemented");
}

void FileStream::AlignByte()
{
    throw exception("Not implemented");
}

byte FileStream::PeekByte() const
{
    assert(GetPosition() < Size());

    int position = ftell(_pFile);
    byte b;
    fread(&b, 1, 1, _pFile);
    fseek(_pFile, position, SEEK_SET);
    return b;
}

void FileStream::ReadBytes(void* pBuffer, int size)
{
    assert(GetPosition() + size <= Size());
    fread(pBuffer, 1, size, _pFile);
}

uint FileStream::ReadBits(int count)
{
    throw exception("Not implemented");
}

void FileStream::Write(const void* pData, int size)
{
    fwrite(pData, 1, size, _pFile);
}
