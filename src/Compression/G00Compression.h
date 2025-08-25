#pragma once

class G00Compression
{
public:
    static std::shared_ptr<MemoryStream>    Compress0       (Stream* pPlain);
    static std::shared_ptr<MemoryStream>    Decompress0     (Stream* pCompressed, int uncompressedSize);

    static std::shared_ptr<MemoryStream>    Compress1       (Stream* pPlain);
    static std::shared_ptr<MemoryStream>    Decompress1     (Stream* pCompressed, int uncompressedSize);
};
