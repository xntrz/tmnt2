#pragma once


class CHitCatchData;
class CGameObject;


class CHitAttackData : public CListNode<CHitAttackData>
{
public:
    enum SHAPE
    {
        SHAPE_SPHERE = 0,
        SHAPE_LINE,
    };

    enum TARGET
    {
        TARGET_NONE                 = 0x0,
        TARGET_PLAYER               = 0x1,
        TARGET_ENEMY                = 0x2,
        TARGET_GIMMICK              = 0x4,
        TARGET_SHOT                 = 0x8,
        TARGET_SELF                 = 0x10,

        TARGET_PLAYER_GIMMICK       = TARGET_PLAYER | TARGET_GIMMICK,
        TARGET_ENEMY_GIMMICK        = TARGET_ENEMY | TARGET_GIMMICK,
        TARGET_CHARACTER            = TARGET_PLAYER | TARGET_ENEMY,
        TARGET_CHARACTER_GIMMICK    = TARGET_CHARACTER | TARGET_GIMMICK,
        TARGET_ALL                  = TARGET_PLAYER | TARGET_ENEMY | TARGET_GIMMICK | TARGET_SHOT | TARGET_SELF,
        TARGET_ALL_EXCEPT_SELF      = TARGET_PLAYER | TARGET_ENEMY | TARGET_GIMMICK | TARGET_SHOT,
    };

    enum EFFECT
    {
        EFFECT_NONE = 0,
        EFFECT_SMALL,
        EFFECT_LARGE,
        EFFECT_STUN,
    };
 
    enum STATUS
    {
        //
        //  Status parameters: IGNORED
        //
        STATUS_KNOCK = 0,

        //
        //  Status parameters:
        //  [0] = velXZ
        //  [1] = velY
        // 
        STATUS_FLYAWAY,

        //
        //  Status parameters: IGNORED
        //
        STATUS_THROW,
        
        //
        //  Status parameters:
        //  [0] = duration
        // 
        STATUS_STUN,
        STATUS_DINDLE,
        STATUS_SLEEP,
        STATUS_FREEZE,
        STATUS_BIND,
    };

    enum ANTIGUARD
    {
        ANTIGUARD_ENABLE = 0,
        ANTIGUARD_BREAK,
        ANTIGUARD_INVALID,
    };

public:
    CHitAttackData(void);
    ~CHitAttackData(void);
    void Cleanup(void);
    void CopyData(CHitAttackData* pHitAttackData) const;
    void SetShape(SHAPE shape);
    void SetSphere(const RwSphere* pSphere);
    void SetLine(const RwLine* pLine);
    void SetObject(uint32 hObj);
    void SetObjectPos(const RwV3d* pPos);
    void SetTarget(TARGET target);
    void SetAttackNo(int32 no);
    void SetMotion(const char* pszMotion);
    void SetPower(int32 amount);
    void SetAntiguard(ANTIGUARD antiguard);
    void SetStatus(STATUS status);
    void SetStatusParameter(float param1, float param2);
    void SetFlyawayParameter(float velXZ, float velY);
    void SetTroubleParameter(float duration);
    void SetFlagConfusion(bool bSet);
    void SetFlagSlice(bool bSet);
    void SetFlagGuardImpact(bool bSet);
    void SetFlagReflectShot(bool bSet);
    void SetFlagAllocated(bool bSet);
    void SetFlagAlive(bool bSet);
    void SetEffect(EFFECT effect);
    void SetHitDistance(float dist);
    void SetCatch(CHitCatchData* pCatch);
    SHAPE GetShape(void) const;
    const RwSphere* GetSphere(void) const;
    const RwLine* GetLine(void) const;
    CGameObject* GetObject(void) const;
    CGameObject* GetCatchObject(void) const;
    uint32 GetObjectHandle(void) const;
    const RwV3d* GetObjectPos(void) const;
    TARGET GetTarget(void) const;
    int32 GetAttackNo(void) const;
    const char* GetMotion(void) const;
    int32 GetPower(void) const;
    ANTIGUARD GetAntiguard(void) const;
    STATUS GetStatus(void) const;
    float GetStatusDuration(void) const;
    float GetFlyawayVelXZ(void) const;
    float GetFlyawayVelY(void) const;
    bool IsTroubleAttack(void) const;
    bool IsFlagConfusion(void) const;
    bool IsFlagSlice(void) const;
    bool IsFlagGuardImpact(void) const;
    bool IsFlagReflectShot(void) const;
    bool IsFlagAllocated(void) const;
    bool IsFlagAlive(void) const;
    EFFECT GetEffect(void) const;
    float GetHitDistance(void) const;
    CHitCatchData* GetCatch(void) const;
    const RwV3d* GetHitPos(void) const;

private:
    uint32 m_uObjectHandle;
    RwV3d m_posObj;
    TARGET m_target;
    int32 m_nAttackNo;
    int32 m_nPower;
    char m_szMotion[16];
    SHAPE m_shapeType;
    union
    {
        RwSphere m_sphere;
        RwLine m_line;
    } m_shape;    
    ANTIGUARD m_antiguard;
    STATUS m_status;
    EFFECT m_effect;
    float m_afStatusParametr[2];
    float m_fHitDist;
    CHitCatchData* m_pHitCatchData;
    bool m_bConfusion;
    bool m_bSlice;
    bool m_bGuardImpact;
    bool m_bReflectShot;
    bool m_bNewAllocated;
    bool m_bAlive;
};