#pragma once

#include "Model.hpp"
#include "ToonManager.hpp"


//
//  TODO rp toon
//
struct RpToonInk;
struct RpToonPaint;


class CModelToon final : public CModel, public CListNode<CModelToon>
{
public:
    CModelToon(const char* pszName, RpClump* pClump, CToonManager::PATTERN pattern);
    virtual ~CModelToon(void);
    virtual void Draw(void) override;
    virtual CModelManager::MODELTYPE GetType(void) const override;
    void SetToonObject(RpToonInk* pInk, RpToonPaint* pPaint);
    CToonManager::PATTERN GetPattern(void) const;

private:
    static float m_fDefaultThickness;
    static RwRGBA m_DefaultColor;

    float m_fSilhouetteThickness;
    RwRGBA m_SilhouetteColor;
    RpToonInk* m_pRpToonInk;
    RpToonPaint* m_pRpToonPaint;
    CToonManager::PATTERN m_pattern;
};