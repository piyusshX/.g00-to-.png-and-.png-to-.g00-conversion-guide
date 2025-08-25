#pragma once

class MemoryStream : public Stream
{
private:
    byte*           _pBuffer;
    int             _size;
    int             _capacity;
    int             _position;
    int             _bitOffset;

public:
                    MemoryStream            ();
                    MemoryStream            (int capacity);
                    MemoryStream            (const void* pData, int size, bool readOnly = false);
    virtual         ~MemoryStream           ();

    void*           Data                    () const;

    virtual int     GetPosition             () const override;
    virtual void    SetPosition             (int position) override;
    virtual void    Seek                    (int offset) override;

    virtual int     Size                    () const override;
    void            Resize                  (int size);

    int             Capacity                () const;
    void            Reserve                 (int capacity);

    virtual void    ReadBytes               (void* pBuffer, int size) override;
    virtual uint    ReadBits                (int count) override;
    virtual byte    PeekByte                () const override;

    virtual void    SkipBits                (int count) override;
    virtual void    AlignByte               () override;

    virtual void    Write                   (const void* pData, int size) override;
    using Stream::Write;
};
