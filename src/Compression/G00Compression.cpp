#include "stdafx.h"

using namespace std;

shared_ptr<MemoryStream> G00Compression::Compress0(Stream* pPlain)
{
    vector<char> plain(pPlain->Size() - pPlain->GetPosition());
    pPlain->ReadBytes(plain.data(), plain.size());

    AVG32Comp::Compress<AVG32Comp::CInfoG00, AVG32Comp::Container::RLDataContainer> compressor;
    compressor.WriteData(plain.data(), plain.size());
    compressor.WriteDataEnd();
    compressor.Deflate();
    compressor.Flush();

    shared_ptr<MemoryStream> pCompressed = shared_ptr<MemoryStream>(new MemoryStream(compressor.Length()));
    pCompressed->Write(compressor.Data(), compressor.Length());
    pCompressed->SetPosition(0);
    return pCompressed;
}

shared_ptr<MemoryStream> G00Compression::Decompress0(Stream* pCompressed, int uncompressedSize)
{
    vector<byte> compressed(pCompressed->Size() - pCompressed->GetPosition());
    pCompressed->ReadBytes(compressed.data(), compressed.size());

    shared_ptr<MemoryStream> pDecompressed = shared_ptr<MemoryStream>(new MemoryStream(uncompressedSize));

    byte* pSource = compressed.data();
    byte* pSourceEnd = pSource + compressed.size();
    byte* pOutBegin = (byte*)pDecompressed->Data();
    byte* pOut = pOutBegin;
    byte* pOutEnd = pOutBegin + uncompressedSize;
    int bit = 1;
    byte flag = *pSource++;
    while (pSource < pSourceEnd && pOut < pOutEnd)
    {
        if (bit == 256)
        {
            flag = *pSource++;
            bit = 1;
        }

        if (flag & bit)
        {
            *pOut++ = *pSource++;
            *pOut++ = *pSource++;
            *pOut++ = *pSource++;
        }
        else
        {
            ushort repetition = *(ushort *)pSource;
            pSource += 2;

            int offset = (repetition >> 4) * 3;
            int size = ((repetition & 0x0F) + 1) * 3;
            
            byte* pRepeat = pOut - offset;
            if (pRepeat < pOutBegin || pRepeat >= pOut)
                throw exception("Malformed file");

            for (int i = 0; i < size && pOut < pOutEnd; ++i)
            {
                *pOut++ = *pRepeat++;
            }
        }
        bit <<= 1;
    }

    pDecompressed->Resize(pOut - pOutBegin);
    return pDecompressed;
}

shared_ptr<MemoryStream> G00Compression::Compress1(Stream* pPlain)
{
    vector<char> plain(pPlain->Size() - pPlain->GetPosition());
    pPlain->ReadBytes(plain.data(), plain.size());

    AVG32Comp::Compress<AVG32Comp::CInfoRealLive, AVG32Comp::Container::RLDataContainer> compressor;
    compressor.WriteData(plain.data(), plain.size());
    compressor.WriteDataEnd();
    compressor.Deflate();
    compressor.Flush();

    shared_ptr<MemoryStream> pCompressed = shared_ptr<MemoryStream>(new MemoryStream(compressor.Length()));
    pCompressed->Write(compressor.Data(), compressor.Length());
    pCompressed->SetPosition(0);
    return pCompressed;
}

shared_ptr<MemoryStream> G00Compression::Decompress1(Stream* pCompressed, int uncompressedSize)
{
    vector<byte> compressed(pCompressed->Size() - pCompressed->GetPosition());
    pCompressed->ReadBytes(compressed.data(), compressed.size());

    shared_ptr<MemoryStream> pDecompressed = make_shared<MemoryStream>(uncompressedSize);

    byte* pSource = compressed.data();
    byte* pSourceEnd = pSource + compressed.size();
    byte* pOutBegin = (byte *)pDecompressed->Data();
    byte* pOut = pOutBegin;
    byte* pOutEnd = pOut + uncompressedSize;
    int bit = 1;
    byte flag = *pSource++;
    while (pSource < pSourceEnd && pOut < pOutEnd)
    {
        if (bit == 256)
        {
            bit = 1;
            flag = *pSource++;
        }

        if (flag & bit)
        {
            *pOut++ = *pSource++;
        }
        else
        {
            ushort repetition = *(ushort *)pSource;
            pSource += 2;

            int offset = repetition >> 4;
            int size = (repetition & 0x0F) + 2;

            byte* rp = pOut - offset;
            if (rp < pOutBegin || rp >= pOut)
                throw exception("Malformed file");

            for (int i = 0; i < size && pOut < pOutEnd; ++i)
            {
                *pOut++ = *rp++;
            }
        }
        bit <<= 1;
    }

    pDecompressed->Resize(pOut - pOutBegin);
    return pDecompressed;
}
