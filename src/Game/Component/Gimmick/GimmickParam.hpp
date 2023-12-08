#pragma once


namespace GIMMICKPARAM
{
    struct QUATERNION
    {
        float x;
        float y;
        float z;
        float w;
    };

    struct GIMMICK_COMMON_HEAD
    {
        int32 m_id;
        int32 m_subid;
        char m_szName[16];
        int32 m_size;
        RwV3d m_vPosition;
        QUATERNION m_quat;
    };

    struct GIMMICK_BASIC : public GIMMICK_COMMON_HEAD
    {
        ;
    };

    struct GIMMICK_GROUP : public GIMMICK_BASIC
    {
        int32 m_nGimmickNum;
        char m_aszGimmickName[0][16];
    };

    struct GIMMICK_TERMS : public GIMMICK_BASIC
    {
        char m_szTargetGimmick[16];
    };

    struct GIMMICK_TERMS_AND : public GIMMICK_TERMS
    {
        char m_szEventGimmickName[16];
    };

    struct GIMMICK_PLAYSE : public GIMMICK_BASIC
    {
        int32 m_nSE;
    };

    struct GIMMICK_HELP : public GIMMICK_BASIC
    {
        int32 m_nHelpNo;
    };

    struct GIMMICK_TUTORIAL : public GIMMICK_BASIC
    {
        int32 m_nTutoNo;
    };

    struct GIMMICK_TERMS_GENERATOR : public GIMMICK_TERMS
    {
        char m_szObserveGeneratorName[16];
        int32 m_nNumTermsDeathEnemy;
    };

    struct GIMMICK_TERMS_TARGET : public GIMMICK_TERMS
    {
        char m_szObserveTargetName[16];
    };

    struct GIMMICK_TERMS_TIME : public GIMMICK_TERMS
    {
        float m_fTime;
    };

    struct GIMMICK_TERMS_AREA_BOX : public GIMMICK_TERMS
    {
        RwV3d m_box;
    };

    struct GIMMICK_TERMS_AREA_SPHERE : public GIMMICK_TERMS
    {
        float m_fRadius;
    };

    struct GIMMICK_TERMS_AREA_WALL : public GIMMICK_TERMS
    {
        RwV2d m_wall;
    };

    struct GIMMICK_ITEMBOX : public GIMMICK_BASIC
    {
        int32 m_nItem;
    };

    struct GIMMICK_AREA_BOX : public GIMMICK_BASIC
    {
        RwV3d m_box;
    };

    struct GIMMICK_MOVEFLOOR_LINEAR : public GIMMICK_BASIC
    {
        int32 m_kind;
        RwV3d m_vPosStart;
        RwV3d m_vPosGoal;
        float m_fMoveTime;
        float m_fStopTime;
    };

    struct GIMMICK_MOVEFLOOR_PATH : public GIMMICK_BASIC
    {
        int32 m_kind;
        char m_szPathName[32];
        float m_fMoveTime;
        float m_fStopTime;
    };

    CHECK_SIZE(GIMMICK_MOVEFLOOR_PATH, 0x64);

    struct GIMMICK_MOVEFLOOR_ROUND : public GIMMICK_BASIC
    {
        RwV3d m_vRotAxis;
        float m_fRoundTime;
    };

    struct GIMMICK_GUILLOTINE : public GIMMICK_BASIC
    {
        float m_fAmplitude;
        float m_fRoundTime;
        float m_fRadius;
    };

    struct GIMMICK_ENEMY_PLACE : public GIMMICK_BASIC
    {
        int32 m_enemyID;
        int32 m_enemyPattern;
        float m_fEnemyRange;
    };

    struct GIMMICK_ENEMY_APPEARANCE_SINGLE : public GIMMICK_BASIC
    {
        int32 m_numTotal;
        float m_fIntervalTime;
        int32 m_numAppearMax;
        int32 m_enemyID;
        int32 m_enemyPattern;
        float m_fEnemyRange;
    };

    struct GIMMICK_ENEMY_APPEARANCE_MULTIPLE : public GIMMICK_BASIC
    {
        int32 m_numTotal;
        float m_fIntervalTime;
        int32 m_numAppearanceEnemyMax;
        int32 m_numAppearanceEnemyOnSameTime;
        int32 m_enemySetID;
        float m_fEnemyRange;
    };
};