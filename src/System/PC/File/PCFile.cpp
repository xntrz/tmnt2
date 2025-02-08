#include "PCFile.hpp"
#include "PCFileManager.hpp"


bool CPCFile::Open(const char* fname)
{
    return CFile::Open(CPCFileManager::FILETYPE_PC, const_cast<char*>(fname));
};