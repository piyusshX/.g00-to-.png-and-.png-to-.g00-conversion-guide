#pragma once

class Path
{
public:
    static std::wstring     Combine             (const std::wstring& path1, const std::wstring& path2);

    static std::wstring     GetDirectoryName    (const std::wstring& filePath);
    static std::wstring     GetFileName         (const std::wstring& filePath);
    static std::wstring     GetExtension        (const std::wstring& filePath);
    static std::wstring     ChangeExtension     (const std::wstring& filePath, const std::wstring& extension);

private:
    static int              GetExtensionIndex   (const std::wstring& filePath);
};
