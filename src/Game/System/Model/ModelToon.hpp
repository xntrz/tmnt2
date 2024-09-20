#pragma once

#include "Model.hpp"
#include "ToonManager.hpp"


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
	
	inline CToonManager::PATTERN GetPattern(void) const { return m_pattern; };
	inline void SetThickness(float f) { m_fSilhouetteThickness = f; };

private:
    static float m_fDefaultThickness;
    static RwRGBA m_DefaultColor;

    float m_fSilhouetteThickness;
    RwRGBA m_SilhouetteColor;
    RpToonInk* m_pRpToonInk;
    RpToonPaint* m_pRpToonPaint;
    CToonManager::PATTERN m_pattern;
};