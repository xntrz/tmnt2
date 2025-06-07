#pragma once

#include "Module.hpp"


class CCloth;
class CModel;
class CCharacter;


class CBandanaModule : public IModule
{
public:
    enum BANDANACOLOR
    {
        BANDANACOLOR_RED = 0,
        BANDANACOLOR_ORANGE,
        BANDANACOLOR_PURPLE,
        BANDANACOLOR_BLUE,
        BANDANACOLOR_BLACK,
        BANDANACOLOR_DARKGREY,
        BANDANACOLOR_DARKBLUE,
        BANDANACOLOR_SHREDDERGREY,
        BANDANACOLOR_SLASHUURGREY,
        BANDANACOLOR_DARKRED,
        
        BANDANACOLORMAX,
    };
    
public:
    CBandanaModule(CCharacter* pCharacter, int32 nBoneID, const RwV3d* pvOffset, BANDANACOLOR color);
    CBandanaModule(CCharacter* pCharacter, CModel* pModel, int32 nBoneID, const RwV3d* pvOffset, BANDANACOLOR color);
    virtual ~CBandanaModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    void Reset(const RwV3d* pvPosition);
    void SetWind(const RwV3d* pvWind);

private:
    CCharacter* m_pCharacter;
    CModel* m_pModel;
    CCloth* m_pCloth;
    int32 m_nBoneID;
    RwV3d m_vOffset;
    RwV3d m_vJointPosition;
    bool m_bWindChrSync;
};