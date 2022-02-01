#pragma once

#include "Module.hpp"


class CPlayerCharacter;


class CFootprintsModule : public IModule
{
public:
    static CFootprintsModule* New(CPlayerCharacter* pPlayerChr);

protected:
    enum FOOTTYPE
    {
        FOOTTYPE_LEFT = 0,
        FOOTTYPE_RIGHT,
    };

    struct WORK : public CListNode<WORK>
    {
        RwV3d m_vPosition;
        RwMatrix m_matrix;
        FOOTTYPE m_foottype;
        uint8 m_uAlphaBasis;

        void Clear(void);
    };

    CFootprintsModule(CPlayerCharacter* pPlayerChr, const RwV2d* pvSize, float fRadius);

public:    
    virtual ~CFootprintsModule(void);
    virtual void Run(void) override;
    virtual void Draw(void) override;
    void Stamp(const RwV3d* pvPosition, float fDirection, FOOTTYPE foottype);
    void SetVertex(void);
    void DrawFootprints(void);

protected:    
    WORK* GetWork(void);

protected:
    CPlayerCharacter* m_pPlayerChr;
    WORK m_aWork[6];
    CList<WORK> m_listWorkPool;
    CList<WORK> m_listWorkAlloc;
    RwIm3DVertex m_aVertex[6 * 6];
    int32 m_nDispNum;
    RwTexture* m_pTexture;
    RwV2d m_vFootSize;
    float m_fRadius;
    FOOTTYPE m_lastFoottype;
    float m_fTime;
};