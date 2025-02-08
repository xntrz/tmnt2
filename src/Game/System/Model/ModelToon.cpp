#include "ModelToon.hpp"
#include "ToonManager.hpp"

#include "rptoon.h"


class CSetToonObjectFunctor final : public CModelToon::IAtomicCallbackFunctor
{
public:
    virtual RpAtomic* operator()(RpAtomic* pAtomic) override;

    RpToonPaint* m_pRpToonPaint;
    RpToonInk* m_pRpToonInk;
};


RpAtomic* CSetToonObjectFunctor::operator()(RpAtomic* pAtomic)
{
	RpGeometry* pGeometry = RpAtomicGetGeometryMacro(pAtomic);
    ASSERT(pGeometry);

	if (RpSkinGeometryGetSkin(pGeometry))
		RpToonSkinAtomicSetType(pAtomic, rpSKINTYPETOON);
	else
		RpToonAtomicEnable(pAtomic);
    
    RpToonGeo* pToonGeometry = RpToonGeometryGetToonGeo(pGeometry);
    ASSERT(pToonGeometry);
    
    if (m_pRpToonInk)
    {
        if (!RpToonGeoGetCreaseInk(pToonGeometry))
            RpToonGeoSetCreaseInk(pToonGeometry, m_pRpToonInk);
        
        if (!RpToonGeoGetSilhouetteInk(pToonGeometry))
            RpToonGeoSetSilhouetteInk(pToonGeometry, m_pRpToonInk);
    };
    
    if (m_pRpToonPaint)
        RpToonGeoSetPaint(pToonGeometry, m_pRpToonPaint);
    
    return pAtomic;
};


/*static*/ float CModelToon::m_fDefaultThickness = 1.0f;
/*static*/ RwRGBA CModelToon::m_DefaultColor = { 0x00, 0x00, 0x00, 0xFF };


CModelToon::CModelToon(const char* pszName, RpClump* pClump, CToonManager::PATTERN pattern)
: CModel(pszName, pClump)
, m_fSilhouetteThickness(m_fDefaultThickness)
, m_SilhouetteColor(m_DefaultColor)
, m_pRpToonInk(nullptr)
, m_pRpToonPaint(nullptr)
, m_pattern(pattern)
{
    CToonManager::RegistModel(this);
};


CModelToon::~CModelToon(void)
{
    CToonManager::RemoveModel(this);
};


void CModelToon::Draw(void)
{
    if (m_pRpToonInk)
    {
		RpToonInkSetOverallThickness(m_pRpToonInk, m_fSilhouetteThickness);
        RpToonInkSetColor(m_pRpToonInk, m_SilhouetteColor);
    };

    CModel::Draw();
};


CModelManager::MODELTYPE CModelToon::GetType(void) const
{
    return CModelManager::MODELTYPE_TOON;
};


void CModelToon::SetToonObject(RpToonInk* pInk, RpToonPaint* pPaint)
{
    RpToonInk* ink = nullptr;
    RpToonPaint* paint = nullptr;

    if (m_pRpToonInk != pInk)
    {
        m_pRpToonInk = pInk;
        ink = m_pRpToonInk;
    };

    if (m_pRpToonPaint != pPaint)
    {
        m_pRpToonPaint = pPaint;
        paint = m_pRpToonPaint;
    };

    CSetToonObjectFunctor functor;
    functor.m_pRpToonInk = ink;
    functor.m_pRpToonPaint = paint;
    
    ForAllAtomicCallback(&functor);
};