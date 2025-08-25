#include "stdafx.h"

Stream::Stream()
{
    _readOnly = false;
}

Stream::~Stream()
{
}

bool Stream::IsAtEnd() const
{
    return GetPosition() >= Size();
}

bool Stream::IsReadOnly() const
{
    return _readOnly;
}

char Stream::ReadChar()
{
    char c;
    ReadAny(c);
    return c;
}

wchar_t Stream::ReadWChar()
{
    wchar_t c;
    ReadAny(c);
    return c;
}

byte Stream::ReadByte()
{
    byte uc;
    ReadAny(uc);
    return uc;
}

short Stream::ReadInt16()
{
    short s;
    ReadAny(s);
    return s;
}

ushort Stream::ReadUInt16()
{
    ushort us;
    ReadAny(us);
    return us;
}

int Stream::ReadInt32()
{
    int i;
    ReadAny(i);
    return i;
}

uint Stream::ReadUInt32()
{
    uint ui;
    ReadAny(ui);
    return ui;
}

int Stream::ReadVarInt32()
{
    int result = ReadByte();
    if ((result & 0x80) == 0)
        return result;

    result = ((result & 0x7F) << 8) | ReadByte();
    if ((result & 0x4000) == 0)
        return result;

    byte uc1 = ReadByte();
    byte uc0 = ReadByte();
    result = ((result & 0x3FFF) << 16) | (uc1 << 8) | uc0;
    return result;
}

__int64 Stream::ReadInt64()
{
    __int64 i;
    ReadAny(i);
    return i;
}

unsigned __int64 Stream::ReadUInt64()
{
    unsigned __int64 ui;
    ReadAny(ui);
    return ui;
}

float Stream::ReadSingle()
{
    float f;
    ReadAny(f);
    return f;
}

double Stream::ReadDouble()
{
    double d;
    ReadAny(d);
    return d;
}

std::wstring Stream::ReadUtf8StringZ()
{
    std::string str;
    char c;
    while (( c = ReadChar()) != '\0')
    {
        str += c;
    }
    
    return Utf8StringToWide(str);
}

std::wstring Stream::ReadUtf8String(int length)
{
    std::string str;
    str.resize(length);
    ReadBytes(const_cast<char*>(str.data()), length);
    return Utf8StringToWide(str);
}

std::wstring Stream::ReadVarUtf8String()
{
    return ReadUtf8String(ReadVarInt32());
}

std::wstring Stream::ReadUtf16StringZ()
{
    std::wstring wstr;
    wchar_t c;
    while ((c = ReadWChar()) != L'\0')
    {
        wstr += c;
    }
    return wstr;
}

std::wstring Stream::ReadUtf16String(int length)
{
    std::wstring wstr;
    wstr.resize(length);
    ReadBytes(const_cast<wchar_t*>(wstr.data()), length * sizeof(wchar_t));
    return wstr;
}

std::wstring Stream::ReadVarUtf16String()
{
    return ReadUtf16String(ReadVarInt32());
}

void Stream::Write(char value)
{
    WriteAny(value);
}

void Stream::Write(wchar_t value)
{
    WriteAny(value);
}

void Stream::Write(byte value)
{
    WriteAny(value);
}

void Stream::Write(short value)
{
    WriteAny(value);
}

void Stream::Write(ushort value)
{
    WriteAny(value);
}

void Stream::Write(int value)
{
    WriteAny(value);
}

void Stream::Write(uint value)
{
    WriteAny(value);
}

void Stream::WriteVar(int value)
{
    if (value <= 0x7F)
    {
        Write((byte)value);
    }
    else if (value <= 0x3FFF)
    {
        Write((byte)(0x80 | (value >> 8)));
        Write((byte)value);
    }
    else
    {
        Write((byte)(0xC0 | (value >> 24)));
        Write((byte)(value >> 16));
        Write((byte)(value >> 8));
        Write((byte)value);
    }
}

void Stream::Write(__int64 value)
{
    WriteAny(value);
}

void Stream::Write(unsigned __int64 value)
{
    WriteAny(value);
}

void Stream::Write(float value)
{
    WriteAny(value);
}

void Stream::Write(double value)
{
    WriteAny(value);
}

void Stream::WriteUtf8StringZ(const std::wstring& wstr)
{
    std::string str = WideStringToUtf8 ( wstr );
    Write(str.data(), str.size());
    Write('\0');
}

void Stream::WriteVarUtf8String(const std::wstring& wstr)
{
    std::string str = WideStringToUtf8(wstr);
    WriteVar(str.size());
    Write(str.data(), str.size());
}

void Stream::WriteUtf16StringZ(const std::wstring& wstr)
{
    Write(wstr.data(), wstr.size() * sizeof(wchar_t));
    Write(L'\0');
}

void Stream::WriteVarUtf16String(const std::wstring& wstr)
{
    WriteVar(wstr.size());
    Write(wstr.data(), wstr.size () * sizeof(wchar_t));
}

void Stream::Write(Stream& stream)
{
    int bytesLeft = stream.Size() - stream.GetPosition();
    byte buffer[0x1000];
    while (bytesLeft > 0)
    {
        int chunkSize = std::min(bytesLeft, (int)sizeof(buffer));
        stream.ReadBytes(buffer, chunkSize);
        Write(buffer, chunkSize);
        bytesLeft -= chunkSize;
    }
}

std::wstring Stream::Utf8StringToWide(const std::string& str)
{
    std::wstring wstr;
    wstr.resize(MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), nullptr, 0));
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), const_cast<wchar_t*>(wstr.data()), wstr.size());
    return wstr;
}

std::string Stream::WideStringToUtf8(const std::wstring& wstr)
{
    std::string str;
    str.resize(WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), nullptr, 0, nullptr, nullptr));
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.size(), const_cast<char*>(str.data()), str.size(), nullptr, nullptr);
    return str;
}
