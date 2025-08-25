#include "stdafx.h"

using namespace std;

bool File::Exists(const wstring& filePath)
{
    DWORD attributes = GetFileAttributes(filePath.c_str());
    if (attributes == -1)
        return false;

    return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

shared_ptr<FileStream> File::Create(const wstring& filePath)
{
    FILE* pFile = _wfopen(filePath.c_str(), L"wb+");
    if (pFile == nullptr)
        return nullptr;

    return shared_ptr<FileStream>(new FileStream(pFile));
}

shared_ptr<FileStream> File::Open(const wstring& filePath)
{
    FILE* pFile = _wfopen(filePath.c_str(), L"rb+");
    if (pFile == nullptr)
        return nullptr;

    return shared_ptr<FileStream>(new FileStream(pFile));
}
