#include "stdafx.h"

using namespace std;

wstring Path::Combine(const wstring& path1, const wstring& path2)
{
    wstring result = path1;
    if (!result.empty() && result[result.size() - 1] != L'\\')
        result += L'\\';

    result += path2;
    return result;
}

wstring Path::GetDirectoryName(const wstring& filePath)
{
    int lastSlashPos = filePath.find_last_of(L'\\');
    if (lastSlashPos < 0)
        return L"";

    return filePath.substr(0, lastSlashPos);
}

wstring Path::GetFileName(const wstring& filePath)
{
    int lastSlashPos = filePath.find_last_of(L'\\');
    if (lastSlashPos < 0)
        return filePath;

    return filePath.substr(lastSlashPos + 1);
}

wstring Path::GetExtension(const wstring& filePath)
{
    int extensionPos = GetExtensionIndex(filePath);
    if (extensionPos < 0)
        return L"";

    return filePath.substr(extensionPos);
}

wstring Path::ChangeExtension(const wstring& filePath, const wstring& extension)
{
    int extensionPos = GetExtensionIndex(filePath);
    if (extensionPos == wstring::npos)
        return filePath;

    return filePath.substr(0, extensionPos) + extension;
}

int Path::GetExtensionIndex(const std::wstring& filePath)
{
    int lastSlashPos = filePath.find_last_of(L'\\');
    int dotPos = filePath.find_last_of(L'.');
    if (dotPos < 0 || (lastSlashPos >= 0 && dotPos < lastSlashPos))
        return wstring::npos;

    return dotPos + 1;
}
