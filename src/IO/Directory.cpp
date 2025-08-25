#include "stdafx.h"

using namespace std;

bool Directory::Exists(const wstring& folderPath)
{
    DWORD attributes = GetFileAttributes(folderPath.c_str());
    if (attributes == -1)
        return false;

    return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}
