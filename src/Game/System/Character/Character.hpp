#pragma once

#include "CharacterTypes.hpp"

#include "Game/System/GameObject/GameObject.hpp"
#include "Game/System/Map/MapTypes.hpp"
#include "Game/Component/Module/ModuleType.hpp"


class CHitAttackData;
class CHitCatchData;
class CCharacterAttackCalculator;
class CModel;
class CMotionParameterController;
class CMotionController;
class CBodyHitData;
class CModuleManager;
class IModule;


class CCharacter : public CGameObject
{
public:
    enum TYPE
    {
        TYPE_PLAYER = 0,
        TYPE_ENEMY,        
    };

    struct PARAMETER
    {
        bool m_bToon;
        const char* m_pszModelName;
        const char* m_pszMotionSetName;
    };

    struct MSG_LIFT_INFO
    {
        RwV3d m_vPosition;
        float m_fDirection;
        int32 m_iStatus;
    };

    struct COLLISION_GIMMICK_INFO
    {
        char m_szGimmickObjName[GAMEOBJECTTYPES::GO_NAME_MAX];
        MAPTYPES::GIMMICKTYPE m_type;
    };

    struct COLLISIONGROUNDINFO
    {
        RwRGBA m_Color;
        MAPTYPES::ATTRIBUTE m_attribute;
        MAPTYPES::HITTYPE m_hittype;
        COLLISION_GIMMICK_INFO m_gimmickinfo;
        bool m_bHit;
    };

    struct COLLISIONWALLINFO
    {
        RwV3d m_vPosition;
        RwV3d m_vNormal;
        RwV3d m_vJumpWallPosition;
        RwV3d m_vJumpWallNormal;
        MAPTYPES::HITTYPE m_hittype;
        MAPTYPES::ATTRIBUTE m_attribute;
        COLLISION_GIMMICK_INFO m_gimmickinfo;
        bool m_bHit;
        bool m_bJumpWallHit;
    };

    struct COLLISIONPARAMETER
    {
        float m_fRadius;
        float m_fHeight;
    };

    struct LIFT_INFO
    {
        char m_szLiftObjectName[GAMEOBJECTTYPES::GO_NAME_MAX];
        int32 m_iStatus;
        int32 m_iDamage;
        bool m_bLiftSuccess;
        bool m_bMissThrow;
    };

public:
    CCharacter(const char* pszName, TYPE type);
    virtual ~CCharacter(void);
    virtual void Run(void) override;
    virtual void MessageProc(int32 nMessageID, void* pParam) override;
    virtual TYPE GetAttackCharacterType(void) const;
    virtual void Draw(void) const;
    virtual void IncludeBasicModule(void);
    virtual void UpdateParameter(void);
    virtual void CalcVelocityPerFrame(RwV3d& rvVelocityPerFrame);
    virtual void CheckCollisionForBody(RwV3d& rvVelocityPerFrame, bool bPeriod);
    virtual void CheckCollisionForWall(RwV3d& rvVelocityPerFrame, bool bPeriod);
    virtual void CheckCollisionForGround(void);
    virtual void UpdateModel(void);
    virtual void UpdateCollisionParameterForBody(void);
    virtual void UpdateCollisionParameterForCatch(void);
    virtual void UpdateCollisionParameterForAttack(void);
    virtual void CheckTimingData(void);
    virtual void OnMessageAerial(float fFieldHeight);
    virtual bool OnMessageCanChangingAerial(float fFieldHeight);
    virtual void OnMessageDoChangingAerial(void);
    virtual void OnMessageTouchdown(float fFieldHeight);
    virtual void OnMessageCatchAttack(CHitAttackData* pAttack);
    virtual void OnMessageAttackResult(CHitCatchData* pCatch);
    virtual void OnMessageReceivedDamage(int32 nDamage);
    virtual void CalcAttackPower(CHitAttackData& rAttack);
    virtual CHARACTERTYPES::ATTACKRESULTTYPE OnDamage(CCharacterAttackCalculator& calculator);
    virtual void OnMessageCatch(void* pParam);
    virtual void OnMessageLift(void* pParam);
    virtual void OnMessageThrow(void* pParam);
    virtual void OnMessageMissThrow(void* pParam);
    virtual void OnMessageCombination(void* pParam);
    virtual void OnChangeMotion(void);
    virtual void OnSteppedDeathFloor(void);
    void RequestDamage(int32 iDmgReq);
    int32 GetRequestedDamage(void) const;
    void GetBonePosition(RwV3d* pvPosition, int32 nBoneID, const RwV3d* pvOffset) const;
    void GetBonePositionOfPositionCheck(RwV3d* pvPosition) const;
    void GetOffsetPosition(RwV3d* pvPosition, int32 no) const;
    void GetBonePositionOfGroundCheck(RwV3d* pvPosition) const;
    void Initialize(PARAMETER* pParameter);
    void SetPosition(const RwV3d* pvPosition);
    void SetVelocity(const RwV3d* pvVelocity);
    void SetAcceleration(const RwV3d* pvAcceleration);
    void ResetVelocity(void);
    void ResetAcceleration(void);
    void SetMaxVelocity(float fMaxVel);
    float GetMaxVelocity(void) const;
    void GetBodyPosition(RwV3d* pvPosition) const;
    void GetFootPosition(RwV3d* pvPosition) const;    
    void SetDirection(float fDir);
    void SetDirectionFromVector(const RwV3d* pvTargetDirection);
    void SetDirectionFromModelRotate(void);
    void RotateVectorByDirection(RwV3d* pvOut, const RwV3d* pvIn) const;
    bool IsWallCrashPossible(void) const;
    void ChangeMotion(const char* pszMotionName, bool bForce = false);
    void PlayMotion(void);
    void StopMotion(void);
    bool IsMotionEnd(void) const;
    void SetMotionTime(float fTime);
    void SetMotionCtrlTime(float fTime);
    void ClearMotionCtrlTime(void);
    float GetMotionTime(void) const;
    float GetMotionEndTime(void) const;
    const char* GetMotionName(void) const;
    float GetNextChainMotionConnectTime(void) const;
    void SetEnableBodyHit(bool bEnable);
    void SetEnableCatchHit(bool bEnable);
    void SetEnableBodyHitSelfToOther(bool bEnable);    
    bool IsEnableBodyHitSelfToOther(void) const;
    void UpdateTransformForModel(void);
    void UpdateMatrices(void);
    void SetMotionSpeed(float fMotionSpeed);    
    void SetGimmickInfo(COLLISION_GIMMICK_INFO* pCollGimmickInfo, MAPTYPES::HITTYPE hittype);
    void GetPosition(RwV3d* pvPosition) const;
    void GetVelocity(RwV3d* pvVelocity) const;
    const RwV3d& GetVelocity(void) const;
    void GetAcceleration(RwV3d* pvAcceleration) const;
    float GetDirection(void) const;
    CModel* GetModel(void) const;
    const COLLISIONWALLINFO* GetCollisionWall(void) const;
    const COLLISIONGROUNDINFO* GetCollisionGround(void) const;
    CMotionParameterController* GetMotionParameterController(void) const;
    TYPE GetCharacterType(void) const;
    const COLLISIONPARAMETER& GetCollisionParameter(void) const;
    IModule* GetModule(MODULETYPE::VALUE type) const;
    bool IsModuleIncluded(MODULETYPE::VALUE type) const;
    LIFT_INFO& LiftInfo(void);
    void SetCharacterFlag(CHARACTERTYPES::FLAG flag);
    void ClearCharacterFlag(CHARACTERTYPES::FLAG flag);
    bool TestCharacterFlag(CHARACTERTYPES::FLAG flag) const;
    bool CheckCharacterFlag(CHARACTERTYPES::FLAG flag) const;
    
protected:
    CModuleManager* m_pModuleMan;
    COLLISIONPARAMETER m_collision;
    COLLISIONWALLINFO m_collisionWallInfo;
    COLLISIONGROUNDINFO m_collisionGroundInfo;
    RwV3d m_vPosition;
    RwV3d m_vVelocity;
    RwV3d m_vAcceleration;
    float m_fDirection;
    float m_fMaxVelocity;
    float m_fMotionSpeed;
    TYPE m_chrtype;
    CHARACTERTYPES::FLAG m_cflag;
    CModel* m_pModel;
    CMotionController* m_pMotionController;
    CBodyHitData** m_apBodyHitData;
    int32 m_nNumBodyHitData;
    CMotionParameterController* m_pMotionParameterController;
    LIFT_INFO m_liftinfo;
    int32 m_iDamageRequest;
};