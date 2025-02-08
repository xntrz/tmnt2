#include "SaveLoadData.hpp"
#include "SaveLoadTMNT2.hpp"

#include "Game/Component/GameData/GameData.hpp"


CSaveLoadData::CSaveLoadData(void)
{
    ;
};


CSaveLoadData::~CSaveLoadData(void)
{
    ;
};


void CSaveLoadData::Initialize(const void* pData, uint32 uDataSize)
{
    CSaveLoadDataBase::Initialize(pData, uDataSize);
    m_iVersion = 3;
};


void CSaveLoadData::Initialize(void)
{
    CSaveLoadDataBase::Initialize();

    TMNT2_SAVE_DATA savedata;
    CGameData::Save(savedata);
    
    SetData(3, &savedata, sizeof(savedata));
};


void CSaveLoadData::Terminate(void)
{
	CSaveLoadDataBase::Terminate();
};


void CSaveLoadData::Update(void)
{
    TMNT2_SAVE_DATA savedata;

	ASSERT((m_uSize - sizeof(HEADER)) == sizeof(savedata));
	std::memcpy(&savedata, reinterpret_cast<char*>(m_pData) + sizeof(HEADER), sizeof(savedata));

    if (savedata.IsValid())
        CGameData::Load(savedata);        
};


bool CSaveLoadData::IsValid(void) const
{
    if (!CSaveLoadDataBase::IsValid())
        return false;

    if (CSaveLoadDataBase::GetSize() != (sizeof(TMNT2_SAVE_DATA) + sizeof(HEADER)))
        return false;

    TMNT2_SAVE_DATA savedata;

    ASSERT((m_uSize - sizeof(HEADER)) == sizeof(savedata));
    std::memcpy(&savedata, reinterpret_cast<char*>(m_pData) + sizeof(HEADER), sizeof(savedata));

    return savedata.IsValid();
};


void CSaveLoadData::SetAutosaveFlag(bool bFlag) const
{
    CGameData::Option().Play().SetEnableAutosave(bFlag);
};

