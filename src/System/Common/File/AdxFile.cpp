#include "AdxFile.hpp"
#include "AdxFileManager.hpp"
#include "FileID.hpp"
#include "Filename.hpp"


bool CAdxFileID::Open(int32 fid)
{
    return CFile::Open(CAdxFileManager::FILETYPE_ID, reinterpret_cast<void*>(fid));
};


bool CAdxFileISO::Open(const char* fname)
{
    int32 fid = CFilename::ID(fname);
    if (fid != FILEID::ID_INVALID)
        return CFile::Open(CAdxFileManager::FILETYPE_ID, reinterpret_cast<void*>(fid));
    
    return false;
};