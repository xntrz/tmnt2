#pragma once


class CIniVar
{
public:
    static const uint32 access_write = 0x1;
	static const uint32 access_read = 0x2;
	static const uint32 access_default = access_write;

    class exception final : public std::exception
    {
    public:
        exception(const std::string msg);
        virtual ~exception(void);
        virtual char const* what(void) const override;

    private:
        std::string m_msg;
    };
 
    class key final
    {
    public:
        struct string_buffer
        {
            const char* m_pBuffer;
            uint32 m_uBufferSize;
        };

    public:        
        key(CIniVar& ini);
        ~key(void);
		void remove(void);
        void operator=(bool value);
        void operator=(float value);
        void operator=(int32 value);
        void operator=(uint32 value);
        void operator=(string_buffer strbuf);
        void operator=(const char* value);

        operator bool(void) const;
        operator float(void) const;
        operator int32(void) const;
        operator uint32(void) const;
        operator const char* (void) const;

    private:
        void write_error(const char* msg) const;
        void read_error(const char* msg) const;

    private:
        CIniVar& m_ini;
        const char* m_pszKeyName;
        const char* m_pszSegmentName;
    };

    class segment final
    {
    public:        
        segment(CIniVar& ini);
        ~segment(void);
		void remove(void);
        key operator[](const char* name);
        bool exist(const char* name);

    private:
        CIniVar& m_ini;
    };

public:
	CIniVar(uint32 access = access_default);
    CIniVar(const char* buffer, int32 bufferSize, uint32 access = access_default);
    ~CIniVar(void);
    std::string save(void);
    bool exist(const char* name);
    segment operator[](const char* name);

private:
    bool check_access(uint32 flag) const;

private:
    void* m_handle;
    uint32 m_access;
    const char* m_pszCurKey;
    const char* m_pszCurSegment;
};