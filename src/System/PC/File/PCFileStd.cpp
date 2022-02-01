#include "PCFileStd.hpp"


static int32 s_stdfRef = 0;
static int32 s_stdfFRef = 0;


void stdf_init(void)
{
    if (!s_stdfRef++)
        s_stdfFRef = 0;
};


void stdf_term(void)
{
	if (s_stdfRef)
	{
		--s_stdfRef;
		ASSERT(s_stdfFRef == 0);
	};    
};


void* stdf_open(const char* fname, const char* mode)
{
    void* file = fopen(fname, mode);
    if (file)
        ++s_stdfFRef;
    return file;
};


void stdf_close(void* file)
{
    ASSERT(file);
    ASSERT(s_stdfFRef > 0);
    --s_stdfFRef;
    
    fclose((FILE*)file);
};


uint32 stdf_read(void* file, void* buffer, uint32 bytes)
{
    return fread(buffer, sizeof(uint8), bytes, (FILE*)file);
};


uint32 stdf_write(void* file, const void* buffer, uint32 bytes)
{
    return fwrite(buffer, sizeof(uint8), bytes, (FILE*)file);
};


uint32 stdf_tell(void* file)
{
    return ftell((FILE*)file);
};


void stdf_seek(void* file, int32 offset, stdf_seek_type type)
{
    static const int32 SeekTable[] =
    {
        SEEK_SET,
        SEEK_CUR,
        SEEK_END,
    };

    fseek((FILE*)file, offset, SeekTable[type]);
};


void stdf_sync(void* file)
{
    fflush((FILE*)file);
};


bool stdf_eof(void* file)
{
    return (feof((FILE*)file) > 0);
};

