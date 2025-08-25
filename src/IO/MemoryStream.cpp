#include "stdafx.h"

MemoryStream::MemoryStream()
{
    _pBuffer = nullptr;
    _size = 0;
    _capacity = 0;
    _position = 0;
    _bitOffset = 0;
}

MemoryStream::MemoryStream(int capacity)
    : MemoryStream()
{
    Reserve(capacity);
}

MemoryStream::MemoryStream(const void* pData, int size, bool readOnly)
    : MemoryStream()
{
    assert(size >= 0);
    if (size == 0)
        return;

    _readOnly = readOnly;
    if (readOnly)
    {
        _pBuffer = (byte *)pData;
        _size = size;
    }
    else
    {
        Resize(size);
        memcpy(_pBuffer, pData, size);
    }
}

MemoryStream::~MemoryStream()
{
    if (_pBuffer != nullptr && !IsReadOnly())
        delete[] _pBuffer;
}

void* MemoryStream::Data() const
{
    return _pBuffer;
}

int MemoryStream::GetPosition() const
{
    return _position;
}

void MemoryStream::SetPosition(int position)
{
    assert(position >= 0 && position <= _size);
    _position = position;
    _bitOffset = 0;
}

void MemoryStream::Seek(int offset)
{
    AlignByte();
    assert(_position + offset >= 0 && _position + offset <= _size);
    _position += offset;
}

int MemoryStream::Size() const
{
    return _size;
}

void MemoryStream::Resize(int size)
{
    assert(size >= 0);
    Reserve(size);
    _size = size;
}

int MemoryStream::Capacity() const
{
    return _capacity;
}

void MemoryStream::Reserve(int capacity)
{
    assert(capacity >= 0);
    if (capacity <= _capacity)
        return;

    if (_capacity == 0)
        _capacity = 1;

    while (_capacity < capacity)
        _capacity *= 2;

    byte* pNewBuffer = new byte[_capacity];
    if (_pBuffer != nullptr)
    {
        memcpy(pNewBuffer, _pBuffer, _size);
        delete[] _pBuffer;
    }
    _pBuffer = pNewBuffer;
}

uint MemoryStream::ReadBits(int count)
{
    assert(_position < _size);
    byte curByte = PeekByte() & ( (1 << (8 - _bitOffset)) - 1 );
    if (count < 8 - _bitOffset)
    {
        curByte >>= (8 - _bitOffset) - count;
        _bitOffset += count;
        return curByte;
    }
    dword result = curByte;
    count -= 8 - _bitOffset;
    _bitOffset = 0;
    Seek(1);
    while (count >= 8)
    {
        result = (result << 8) | ReadByte();
        count -= 8;
    }
    if (count != 0)
    {
        assert(_position < _size);
        result = (result << count) | (PeekByte () >> (8 - count));
    }
    _bitOffset = count;
    return result;
}

byte MemoryStream::PeekByte() const
{
    assert(_position < _size);
    return _pBuffer[_position];
}

void MemoryStream::SkipBits(int count)
{
    assert(count >= 0);
    if (count < 8 - _bitOffset)
    {
        _bitOffset += count;
        return;
    }
    count -= 8 - _bitOffset;
    Seek(count / 8);
    _bitOffset = count % 8;
}

void MemoryStream::AlignByte()
{
    if ( _bitOffset != 0 )
    {
        _bitOffset = 0;
        Seek(1);
    }
}

void MemoryStream::ReadBytes(void* pBuffer, int size)
{
    AlignByte();
    assert(_position + size >= 0 && _position + size <= _size);
    memcpy(pBuffer, (byte *)_pBuffer + _position, size);
    _position += size;
}

void MemoryStream::Write(const void* pData, int size)
{
    assert(!IsReadOnly());

    Resize(std::max(_position + size, _size));
    memcpy(&_pBuffer[_position], pData, size);
    _position += size;
}
