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
    enum
    {
        MODELTYPE_DRAW_NORMAL = 0,
        MODELTYPE_ATARI_NORMAL,
        MODELTYPE_DRAW_BREAK,
        MODELTYPE_ATARI_BREAK,
    };
    
public:
    CNormalGimmickModel(void);
    virtual ~CNormalGimmickModel(void);
    virtual void Draw(void) const override;
    virtual void UpdateFrame(void) override;
    void SetModel(int32 idx, CModel* pModel);
    void SetColor(int32 idx, const RwRGBA& rColor);
    RpClump* GetCollisionModelClump(void) const;

    // returns pointer to an modeling matrix
    RwMatrix* GetMatrix(int32 idx) const;

    inline CModel* GetModel(int32 idx) const        { return m_apModel[idx]; };
    inline CModel* GetVisualModel(void) const       { return GetModel(m_idxModelDraw); };
    inline CModel* GetCollisionModel(void) const    { return (GetModel(m_idxModelAtari) ? GetModel(m_idxModelAtari) : GetModel(m_idxModelDraw)); };
    
    inline bool IsDrawEnable(void) const            { return m_bDrawEnable; };
    inline void SetDrawEnable(bool bEnable)         { m_bDrawEnable = bEnable; };
    
    inline int32 GetDrawType(void) const            { return m_idxModelDraw; };
    inline int32 GetAtariType(void) const           { return m_idxModelAtari; };
    inline void SetDrawType(int32 idx)              { m_idxModelDraw = idx; };
    inline void SetAtariType(int32 idx)             { m_idxModelAtari = idx; };
    
    inline void SetVisualNormal(void)               { SetDrawType(MODELTYPE_DRAW_NORMAL); };
    inline void SetCollisionNormal(void)            { SetAtariType(MODELTYPE_ATARI_NORMAL); };
    inline void SetVisualBreak(void)                { SetDrawType(MODELTYPE_DRAW_BREAK); };
    inline void SetCollisionBreak(void)             { SetAtariType(MODELTYPE_ATARI_BREAK); };

protected:
    int32 m_idxModelDraw;
    int32 m_idxModelAtari;
    bool m_bDrawEnable;
};