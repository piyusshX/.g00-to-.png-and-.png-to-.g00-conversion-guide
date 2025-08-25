#pragma once

class FileStream : public Stream
{
private:
    FILE* _pFile = nullptr;

public:
                            FileStream          (FILE* pFile);
    virtual                 ~FileStream         ();

    virtual int             GetPosition         () const override;
    virtual void            SetPosition         (int position) override;

    virtual void            Seek                (int offset) override;

    virtual int             Size                () const override;

    virtual void            SkipBits            (int count) override;
    virtual void            AlignByte           () override;

    virtual byte            PeekByte            () const override;
    virtual void            ReadBytes           (void* pBuffer, int size) override;
    virtual uint            ReadBits            (int count) override;

    virtual void            Write               (const void * pData, int size) override;
    using Stream::Write;
};
