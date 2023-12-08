#include "GimmickModel.hpp"

#include "Game/System/Model/Model.hpp"
#include "Game/System/Model/ModelManager.hpp"


CGimmickModel::CGimmickModel(int32 nNumGimmickModel)
: m_apModel(nullptr)
, m_nNumGimmickModel(nNumGimmickModel)
{
    ASSERT(m_nNumGimmickModel > 0);
    
    m_apModel = new CModel*[m_nNumGimmickModel];
    ASSERT(m_apModel);

    for (int32 i = 0; i < m_nNumGimmickModel; ++i)
        m_apModel[i] = nullptr;
};


CGimmickModel::~CGimmickModel(void)
{
    if (m_apModel)
    {
        for (int32 i = 0; i < m_nNumGimmickModel; ++i)
        {
            if (m_apModel[i])
            {
                CModelManager::DestroyModel(m_apModel[i]);
                m_apModel[i] = nullptr;
            };
        };

        m_nNumGimmickModel = 0;

        delete[] m_apModel;
        m_apModel = nullptr;
    };
};


void CGimmickModel::Draw(void) const
{
    for (int32 i = 0; i < m_nNumGimmickModel; ++i)
    {
        CModel* pModel = m_apModel[i];
        if(pModel)
        {
            if (i)
                pModel->UpdateFrame();
            else
                pModel->Draw();
        };
    };
};


void CGimmickModel::UpdateFrame(void)
{
    for (int32 i = 0; i < m_nNumGimmickModel; ++i)
    {
        CModel* pModel = m_apModel[i];
        if (pModel)
            pModel->UpdateFrame();
    };    
};


void CGimmickModel::SetPosition(const RwV3d* pPos)
{
    for (int32 i = 0; i < m_nNumGimmickModel; ++i)
    {
        CModel* pModel = m_apModel[i];
        if (pModel)
        {
            pModel->SetPosition(pPos);
            pModel->UpdateFrame();
        };
    };
};


void CGimmickModel::SetRotation(const RwV3d* pRot)
{
    for (int32 i = 0; i < m_nNumGimmickModel; ++i)
    {
        CModel* pModel = m_apModel[i];
        if (pModel)
        {
            pModel->SetRotation(pRot);
            pModel->UpdateFrame();
        };
    };
};


void CGimmickModel::GetPosition(RwV3d* pPos) const
{
    ASSERT(pPos);
    ASSERT(m_apModel[0]);
    
    m_apModel[0]->GetPosition(pPos);
};


CNormalGimmickModel::CNormalGimmickModel(void)
: CGimmickModel(4)
, m_idxModelDraw(0)
, m_idxModelAtari(1)
, m_bDrawEnable(true)
{
    ;
};


CNormalGimmickModel::~CNormalGimmickModel(void)
{
    ;
};


void CNormalGimmickModel::Draw(void) const
{
    if (!m_bDrawEnable)
        return;
    
    for (int32 i = 0; i < m_nNumGimmickModel; ++i)
    {
        CModel* pModel = m_apModel[i];
        if (pModel)
        {
            if (i == m_idxModelDraw)
                pModel->Draw();
            else
                pModel->UpdateFrame();            
        };
    };
};


void CNormalGimmickModel::UpdateFrame(void)
{
    CGimmickModel::UpdateFrame();
};


void CNormalGimmickModel::SetModel(int32 idx, CModel* pModel)
{
    ASSERT(idx >= 0 && idx < m_nNumGimmickModel);
    ASSERT(!m_apModel[idx]);
    ASSERT(pModel);
    
    m_apModel[idx] = pModel;
};


void CNormalGimmickModel::SetColor(int32 idx, const RwRGBA& rColor)
{
    ASSERT(idx >= 0 && idx < m_nNumGimmickModel);
    ASSERT(m_apModel[idx]);

    m_apModel[idx]->SetColor(rColor);
};


RpClump* CNormalGimmickModel::GetCollisionModelClump(void) const
{
    return (GetCollisionModel() ? GetCollisionModel()->GetClump() : nullptr);
};


RwMatrix* CNormalGimmickModel::GetMatrix(int32 idx) const
{
    CModel* pModel = GetModel(idx);
    RpClump* pClump = pModel->GetClump();
    RwFrame* pFrame = RpClumpGetFrameMacro(pClump);
    RwMatrix* pMatrix = RwFrameGetMatrixMacro(pFrame);
    
    return pMatrix;
};