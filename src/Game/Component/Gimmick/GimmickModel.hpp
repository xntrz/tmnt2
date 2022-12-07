#pragma once


class CModel;


class CGimmickModel
{
public:
    CGimmickModel(int32 nNumGimmickModel);
    virtual ~CGimmickModel(void);
    virtual void Draw(void) const;
    virtual void UpdateFrame(void);
    void SetPosition(const RwV3d* pPos);
    void SetRotation(const RwV3d* pRot);
    void GetPosition(RwV3d* pPos) const;

protected:
    CModel** m_apModel;
    int32 m_nNumGimmickModel;
};


class CNormalGimmickModel : public CGimmickModel
{
public:
    static const int32 MODELNUM = 4;

    enum MODELKIND
    {
        MODELKIND_VISUAL_NORMAL = 0,
        MODELKIND_VISUAL_BREAK,
        MODELKIND_ATARI_NORMAL,
        MODELKIND_ATARI_BREAK,
        MODELKINDNUM,
    };

public:
    CNormalGimmickModel(void);
    virtual ~CNormalGimmickModel(void);
    virtual void Draw(void) const override;
    virtual void UpdateFrame(void) override;
    void SetModel(int32 modelkind, CModel* pModel);
    CModel* GetModel(int32 modelkind) const;
    CModel* GetVisualModel(void) const;
    CModel* GetCollisionModel(void) const;
    RpClump* GetCollisionModelClump(void) const;
    void SetVisualNormal(void);
    void SetVisualBreak(void);
    void SetCollisionNormal(void);
    void SetCollisionBreak(void);
    void SetDrawEnable(bool bEnable);
    bool IsDrawEnable(void) const;
    void SetColor(int32 modelkind, const RwRGBA& rColor);

protected:
    MODELKIND m_CollisionModelKind;
    MODELKIND m_ViewModelKind;
    bool m_bDrawEnable;
};