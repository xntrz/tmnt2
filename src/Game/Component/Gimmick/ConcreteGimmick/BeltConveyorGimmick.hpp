#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CBeltConveyorGimmick final : public CGimmick
{
private:
    enum SUBID
    {
        SUBID_START = 0,
        SUBID_MODEL_DISP = SUBID_START,
        SUBID_MODEL_NON_DISP,

        SUBID_NUM,
    };

public:
    CBeltConveyorGimmick(const char* pszName, void* pParam);
    virtual ~CBeltConveyorGimmick(void);
    virtual void Draw(void) const override;
    virtual void PostMove(void) override;
    void SetPosition(const RwV3d* pvPosition);
    void SetRotation(const RwV3d* pvRotation);
    
private:
    static bool         m_bAnimOwnerExist;
    CNormalGimmickModel m_model;
    uint32              m_hAtari;
    SUBID               m_subid;
    RwV3d               m_vPosition;
    RwV3d               m_vDirection;
    float               m_fVelocity;
    bool                m_bAnimOwner;
    void*               m_pAnimatedMaterials;
};