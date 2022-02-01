#include "AfsFile.hpp"
#include "AfsFileManager.hpp"
#include "AfsFilename.hpp"
#include "AfsFileID.hpp"


CAfsFile::CAfsFile(int32 nLabel)
: CFile(nLabel)
{
    ;
};


CAfsFile::~CAfsFile(void)
{
    ;
};


CAfsFileID::CAfsFileID(void)
: CAfsFile(CAfsFileManager::LABELAFS_ID)
{
    ;
};


CAfsFileID::~CAfsFileID(void)
{
    ;
};


CAfsFileISO::CAfsFileISO(void)
: CAfsFile(CAfsFileManager::LABELAFS_NAME)
{
    ;
};


CAfsFileISO::~CAfsFileISO(void)
{
    ;
};


bool CAfsFileISO::Open(const char* pszName)
{
	bool bResult = false;

    int32 id = CAfsFilename::FileID(pszName);
	if (id != AFSFILEID::ID_INVALID)
	{
		m_nLabel = CAfsFileManager::LABELAFS_ID;
		bResult = CAfsFile::Open(id);
		m_nLabel = CAfsFileManager::LABELAFS_NAME;
	};
    
	return bResult;
};