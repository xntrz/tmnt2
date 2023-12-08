#pragma once

#include "Module.hpp"


class CGameObject;


class CCircleShadowModule : public IModule
{
public:
    static CCircleShadowModule* New(CGameObject* pGameObject, float w, float h, bool bEnable);
    static void DrawAll(void);
    
protected:
    CCircleShadowModule(float w, float h);
    
public:
    virtual ~CCircleShadowModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    virtual void GetPosition(RwV3d* pvPosition) const = 0;
    virtual float GetDirection(void) const = 0;
    void SetSize(float w, float h);
    void SetEnable(bool bEnable);
    bool IsEnabled(void) const;
    void DrawNode(void);

protected:
    static CList<CCircleShadowModule> m_listDraw;
    CListNode<CCircleShadowModule> m_nodeDraw;
    RwIm3DVertex m_aVertices[4];
    RwMatrix m_matrix;
    RwTexture* m_pTexture;
    bool m_bEnable;
};