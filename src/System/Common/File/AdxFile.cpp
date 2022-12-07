#include "AdxFile.hpp"
#include "AdxFileManager.hpp"
#include "FileID.hpp"
#include "Filename.hpp"


CAdxFile::CAdxFile(int32 nLabel)
: CFile(nLabel)
{
    ;
};


CAdxFile::~CAdxFile(void)
{
    ;
};


CAdxFileID::CAdxFileID(void)
: CAdxFile(CAdxFileManager::LABELADX_ID)
{
    ;
};


CAdxFileID::~CAdxFileID(void)
{
    ;
};


CAdxFileISO::CAdxFileISO(void)
: CAdxFile(CAdxFileManager::LABELADX_NAME)
{
    ;
};


CAdxFileISO::~CAdxFileISO(void)
{
    ;
};


bool CAdxFileISO::Open(const char* pszName)
{
	bool bResult = false;

    int32 id = CFilename::ID(pszName);
	if (id != FILEID::ID_INVALID)
	{
		m_nLabel = CAdxFileManager::LABELADX_ID;
		bResult = CAdxFile::Open(id);
		m_nLabel = CAdxFileManager::LABELADX_NAME;
	};
    
	return bResult;
};