#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CMadCarGimmickMove;


class CMadCarGimmick final : public CGimmick
{
private:
    class CCarHeadLight
    {
    public:
        CCarHeadLight(void);
        ~CCarHeadLight(void);
        void Init(const RwV3d* pvPosition, float fRadius);
        void Term(void);
        void Update(const RwV3d* pvPosition);

    private:
        uint32 m_hLight;
    };

public:
    enum SUBID
    {
        SUBID_START = 0,

        SUBID_START_LIGHON = SUBID_START,        
        SUBID_D21 = SUBID_START_LIGHON,
        SUBID_DEVIL,
        SUBID_IMP59,
        SUBID_MSTU,        
        SUBID_END_LIGHTON,
        
        SUBID_START_LIGHTOFF = SUBID_END_LIGHTON,
        SUBID_D21_LIGHT = SUBID_START_LIGHTOFF,
        SUBID_DEVIL_LIGHT_OFF,
        SUBID_IMP59_LIGHT_OFF,
        SUBID_MSTU_LIGHT_OFF,        
        SUBID_END_LIGHTOFF,
        
        SUBID_NUM = SUBID_END_LIGHTOFF,
    };

public:
    static int32 GetCarType(SUBID subid);
    
    CMadCarGimmick(const char* pszName, void* pParam);
    virtual ~CMadCarGimmick(void);
    virtual void PostMove(void) override;
    
private:
    CNormalGimmickModel m_model;
    CMadCarGimmickMove* m_pMove;
    int32               m_subid;
    char                m_szPathName[16];
    CCarHeadLight       m_aCarLight[2];
    bool                m_bCarLight;
};