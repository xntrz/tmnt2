#pragma once


typedef enum _stdf_seek_type
{
    stdf_seek_begin = 0,
    stdf_seek_current,
    stdf_seek_end,
} stdf_seek_type;


void stdf_init(void);
void stdf_term(void);
void* stdf_open(const char* fname, const char* mode);
void stdf_close(void* file);
uint32 stdf_read(void* file, void* buffer, uint32 bytes);
uint32 stdf_write(void* file, const void* buffer, uint32 bytes);
uint32 stdf_tell(void* file);
void stdf_seek(void* file, int32 offset, stdf_seek_type type);
void stdf_sync(void* file);
bool stdf_eof(void* file);