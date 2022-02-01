#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"


class CModuleManager;


class CItemGimmick : public CGimmick
{
protected:
    enum STATE
    {
        STATE_INIT = 0,
        STATE_FALL,
        STATE_WAIT,
        STATE_END,
    };
    
public:
    CItemGimmick(const char* pszName, void* pParam);
    virtual ~CItemGimmick(void);
    virtual void Draw(void) const override;
    virtual void GetPosition(RwV3d* pvPosition) const override;
    virtual void PostMove(void) override;
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype) override;

protected:
    void init(void* pParam);
    void setVertex(float fRadius, const RwRGBA& rColor);
    void getItem(int32 nPlayerNo);
    bool checkItemGet(void);
    bool isBlinkDisappear(void) const;
    bool isCrystalItem(void) const;
    bool isTimelimitEnable(void) const;

protected:
    int32 m_nItemID;
    RwV3d m_vPosition;
    RwV3d m_vVelocity;
    RwIm3DVertex m_aVertex[4];
    RwTexture* m_pTexture;
    CModuleManager* m_pModuleManager;
    float m_fTimer;
    float m_fRadian;
    float m_fOffsetY;
    float m_fDeathHeight;
    STATE m_state;
    uint32 m_hAppearEffect;
};