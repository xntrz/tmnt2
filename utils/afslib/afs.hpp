#pragma once


typedef enum _afs_note_type
{
    afs_note_error = 0,
    afs_note_msg,    
} afs_note_type;

typedef void(*afs_enum_callback)(int idx, const char* name, unsigned size, unsigned offset);
typedef void(*afs_note_callback)(afs_note_type type, const char* msg);

void afs_opt_excludemask(const char* mask);
void afs_opt_align(unsigned alignment);
void afs_opt_recursive(bool value);
void afs_opt_note(afs_note_callback cb);
void* afs_new(const char* path);
void* afs_open(const char* path);
void afs_close(void* handle);
bool afs_read(void* handle, int idx, void* buffer, unsigned bufferSize);
unsigned afs_fsize(void* handle, int idx);
void afs_enum(void* handle, afs_enum_callback cb);
bool afs_extract(void* handle, const char* path, int idx = -1);