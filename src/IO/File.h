#pragma once

class File
{
public:
    static bool                             Exists          (const std::wstring& filePath);
    static std::shared_ptr<FileStream>      Create          (const std::wstring& filePath);
    static std::shared_ptr<FileStream>      Open            (const std::wstring& filePath);
};
