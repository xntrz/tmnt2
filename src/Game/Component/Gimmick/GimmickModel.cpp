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
    {
        m_apModel[i] = nullptr;
    };
};


CGimmickModel::~CGimmickModel(void)
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

    ASSERT(m_apModel);
    delete[] m_apModel;
    m_apModel = nullptr;
};


void CGimmickModel::Draw(void) const
{
    for (int32 i = 0; i < m_nNumGimmickModel; ++i)
    {
        CModel* pModel = m_apModel[i];
        if(pModel)
        {
            if (i)
            {
                pModel->UpdateFrame();
            }
            else
            {
                pModel->Draw();
            };
        };
    };
};


void CGimmickModel::UpdateFrame(void)
{
    for (int32 i = 0; i < m_nNumGimmickModel; ++i)
    {
        CModel* pModel = m_apModel[i];
        if (pModel)
        {
            pModel->UpdateFrame();
        };
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
: CGimmickModel(MODELNUM)
, m_CollisionModelKind(MODELKIND_ATARI_NORMAL)
, m_ViewModelKind(MODELKIND_VISUAL_NORMAL)
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
            if (i == m_ViewModelKind)
            {
                pModel->Draw();
            }
            else
            {
                pModel->UpdateFrame();
            };
        };
    };
};


void CNormalGimmickModel::UpdateFrame(void)
{
    CGimmickModel::UpdateFrame();
};


void CNormalGimmickModel::SetModel(int32 modelkind, CModel* pModel)
{
    ASSERT(modelkind >= 0 && modelkind < m_nNumGimmickModel);
    ASSERT(modelkind >= 0 && modelkind < MODELKINDNUM);
    ASSERT(!m_apModel[modelkind]);
    ASSERT(pModel);
    
    m_apModel[modelkind] = pModel;
};


CModel* CNormalGimmickModel::GetModel(int32 modelkind) const
{
    return m_apModel[modelkind];
};


CModel* CNormalGimmickModel::GetVisualModel(void) const
{
    return m_apModel[m_ViewModelKind];
};


CModel* CNormalGimmickModel::GetCollisionModel(void) const
{
    if (m_apModel[m_CollisionModelKind])
    {
        return m_apModel[m_CollisionModelKind];
    }
    else
    {
        return m_apModel[m_ViewModelKind];
    };
};


RpClump* CNormalGimmickModel::GetCollisionModelClump(void) const
{
    CModel* pModel = GetCollisionModel();
    if (pModel)
    {
        return pModel->GetClump();
    }
    else
    {
        return nullptr;
    };
};


void CNormalGimmickModel::SetVisualNormal(void)
{
    m_ViewModelKind = MODELKIND_VISUAL_NORMAL;
};


void CNormalGimmickModel::SetVisualBreak(void)
{
    m_ViewModelKind = MODELKIND_VISUAL_BREAK;
};


void CNormalGimmickModel::SetCollisionNormal(void)
{
	m_CollisionModelKind = MODELKIND_ATARI_NORMAL;
};


void CNormalGimmickModel::SetCollisionBreak(void)
{
	m_CollisionModelKind = MODELKIND_ATARI_BREAK;
};


void CNormalGimmickModel::SetDrawEnable(bool bEnable)
{
    m_bDrawEnable = bEnable;
};


bool CNormalGimmickModel::IsDrawEnable(void) const
{
    return m_bDrawEnable;
};


void CNormalGimmickModel::SetColor(int32 modelkind, const RwRGBA& rColor)
{
    ASSERT(m_apModel[modelkind]);
    m_apModel[modelkind]->SetColor(rColor);
};