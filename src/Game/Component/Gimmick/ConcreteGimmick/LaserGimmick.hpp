#pragma once

#include "Game/Component/Gimmick/Gimmick.hpp"
#include "Game/Component/Gimmick/GimmickModel.hpp"


class CLaserGimmickModel : public CGimmickModel
{
public:
    enum TYPE
    {
        TYPE_RED = 0,
        TYPE_PINK,
        TYPE_TRANSPARENT,
    };

public:
    CLaserGimmickModel(void);
    virtual ~CLaserGimmickModel(void);
    virtual void Draw(void) const override;
    void SetLaserForm(const RwV3d* pvPos, const RwV3d* pvDir, float fNorm);
    void SetDraw(bool bEnable);
    void SetType(TYPE type);
    
protected:
    RwV3d m_vPosition;
    RwV3d m_vDirection;
    float m_fNorm;
    TYPE  m_type;
    bool  m_bDraw;
};


class CLaserGimmick : public CGimmick
{
public:
    CLaserGimmick(const char* pszName, void* pParam);
    virtual ~CLaserGimmick(void);
    virtual void GetPosition(RwV3d* pvPosition) const;    
    virtual void PostMove(void);
    virtual void OnReceiveEvent(const char* pszSender, GIMMICKTYPES::EVENTTYPE eventtype);
    void SetLasertForm(const RwV3d* pvPos, const RwV3d* pvDir);
    
protected:
    RwV3d               m_vPosition;
    RwV3d               m_vDirection;
    float               m_fNorm;
    bool                m_bSwitch;
    CLaserGimmickModel  m_model;
    RwLine              m_line;
    int32               m_type;
};