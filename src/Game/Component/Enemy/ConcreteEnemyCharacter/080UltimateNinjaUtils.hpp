#pragma once

#include "Game/Component/Enemy/ConcreteAIModerator/BaseAI6034.hpp"
#include "Game/System/Hit/HitAttackData.hpp"


class CModel;


namespace UltimateNinjaUtils
{
    class CBodyPart
    {
    public:
        CBodyPart(void);
        ~CBodyPart(void);
        void Initialize(const CGameObject* pGameObj, int32 iAttackNo, float fAttackRadius);
        void Update(void);
        void SetHitTypeSleep(void);
        void SetHitTypeFlyaway(void);
        void SetPosition(const RwV3d* pvecPos);

    protected:
        CHitAttackData m_attackData;
        float          m_fAttackRadius;
    };


    class CSplinePath
    {
    public:
        static void GetSplinePos(RwV3d* pvecPos, int32 iPathID, float t);

        CSplinePath(void);
        ~CSplinePath(void);
        void SetPathName(const char* pszPathName);
        void GetPos(RwV3d* pvecPos, float t) const;
        void GetDirection(RwV3d* pvecDir, float t) const;
        float GetLength(void) const;

    protected:
        int32 m_iPathID;
    };


    class CDragonEffect
    {
    public:
        CDragonEffect(void);
        ~CDragonEffect(void);
        void Update(void);
        void Start(void);
        void Stop(void);
        void SetHeadData(const RwV3d* pvecPos, const RwV3d* pvecDir);

    protected:
        Utils6034::CTraceEffect*    m_pBodySmoke;
        Utils6034::CPositionTracer  m_bodyPositionTracer;
        Utils6034::CTraceEffect     m_bodyEffect;
        Utils6034::CTraceMagic*     m_apNeckSmoke[2];
        Utils6034::CPositionTracer  m_aNeckPositionTracer[2];
        Utils6034::CTraceMagic      m_aNeckMagic[2];
    };


    class CDragon
    {
    public:
        CDragon(const CCharacterCompositor* pChrCompositor);
        ~CDragon(void);
        void Update(void);
        void Draw(void);
        void Start(void);
        void Stop(void);
        void SetPathID(int32 iPathID);
        bool IsActive(void) const;
        void GetStartPos(int32 iPathID, RwV3d* pvecPos);
        void GetSplinePos(RwV3d* pvecPos, float t);
        void GetRotationByDirection(RwV3d* pvecRot, const RwV3d* pvecDir);
        void ClearData(void);
        void UpdateData(void);

    protected:
        bool                        m_bActive;
        bool                        m_bDraw;
        CBodyPart                   m_aBodyParts[10];
        CSplinePath                 m_splinePath;
        CModel*                     m_pModel;
        RwV3d                       m_vecHeadPos;
        RwV3d                       m_vecHeadDir;
        float                       m_fCurrentPathT;
        float                       m_fUpdatePathT;
        float                       m_fBodyPosDiffTime;
        CDragonEffect               m_dragonEffect;
        Utils6034::CFixTimer        m_dragonStartTimer;
        Utils6034::CPositionTracer  m_startPositionTracer;
        Utils6034::CTraceMagic      m_startMagic;
        Utils6034::CFixTimer        m_startEffectTimer;
        RwV3d                       m_vecPosition;
        const CCharacterCompositor* m_pChrCompositor;
        float                       m_fMoveSpeedFrame;
    };
};