#pragma once

#include "RideGimmick.hpp"


class CRideFlagGimmick : public CRideGimmick
{
public:
    enum FLAGTYPE
    {
        FLAGTYPE_LOW = 0,
        FLAGTYPE_HIGH_FIXED,
        FLAGTYPE_HIGH_WIDTH_R,
        FLAGTYPE_HIGH_WIDTH_L,
        FLAGTYPE_HIGH_HEIGHT_U,
        FLAGTYPE_HIGH_HEIGHT_D,
        
        FLAGTYPENUM,
    };
    
public:
    static void Initialize(void);

    CRideFlagGimmick(const char* pszName, void* pParam);
    virtual ~CRideFlagGimmick(void);
    virtual void OnAttackResult(CHitCatchData* pCatch) override;

protected:
    static int32 m_iCounter[FLAGTYPENUM];
    static int32 m_iRestCounter[FLAGTYPENUM];
    FLAGTYPE m_flagtype;
    int32 m_iScore;
    bool m_bAdd;
};