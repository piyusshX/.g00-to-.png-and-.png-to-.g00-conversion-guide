#pragma once

class Stream
{
protected:
    bool                _readOnly;

public:
                        Stream                  ();
    virtual             ~Stream                 ();

    virtual int         GetPosition             () const = 0;
    virtual void        SetPosition             (int position) = 0;
    virtual void        Seek                    (int offset) = 0;
    bool                IsAtEnd                 () const;

    virtual int         Size                    () const = 0;

    bool                IsReadOnly              () const;

    virtual void        SkipBits                (int count) = 0;
    virtual void        AlignByte               () = 0;

    virtual byte        PeekByte                () const = 0;

    virtual void        ReadBytes               (void* pBuffer, int size) = 0;
    virtual uint        ReadBits                (int count) = 0;
    char                ReadChar                ();
    wchar_t             ReadWChar               ();
    byte                ReadByte                ();
    short               ReadInt16               ();
    ushort              ReadUInt16              ();
    int                 ReadInt32               ();
    uint                ReadUInt32              ();
    int                 ReadVarInt32            ();
    __int64             ReadInt64               ();
    unsigned __int64    ReadUInt64              ();
    float               ReadSingle              ();
    double              ReadDouble              ();
    std::wstring        ReadUtf8StringZ         ();
    std::wstring        ReadUtf8String          (int length);
    std::wstring        ReadVarUtf8String       ();
    std::wstring        ReadUtf16StringZ        ();
    std::wstring        ReadUtf16String         (int length);
    std::wstring        ReadVarUtf16String      ();

    virtual void        Write                   (const void* pData, int size) = 0;
    void                Write                   (char value);
    void                Write                   (wchar_t value);
    void                Write                   (byte value);
    void                Write                   (short value);
    void                Write                   (ushort value);
    void                Write                   (int value);
    void                Write                   (uint value);
    void                WriteVar                (int value);
    void                Write                   (__int64 value);
    void                Write                   (unsigned __int64 value);
    void                Write                   (float value);
    void                Write                   (double value);
    void                WriteUtf8StringZ        (const std::wstring& wstr);
    void                WriteVarUtf8String      (const std::wstring& wstr);
    void                WriteUtf16StringZ       (const std::wstring& wstr);
    void                WriteVarUtf16String     (const std::wstring& wstr);
    void                Write                   (Stream& stream);

private:
    std::wstring        Utf8StringToWide        (const std::string& str);
    std::string         WideStringToUtf8        (const std::wstring& wstr);

    template<typename T>
    void                ReadAny                 (T& data)
    {
        ReadBytes(&data, sizeof(data));
    }

    template<typename T>
    void                WriteAny                (const T data)
    {
        Write(&data, sizeof(data));
    }
};
