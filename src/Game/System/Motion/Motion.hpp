#pragma once


class CMotion : public CListNode<CMotion>
{
public:
    CMotion(const char* pszName, RtAnimAnimation* pHAnimation);
    
    inline ~CMotion(void) {};
    
    inline const char* GetName(void) const              { return m_pszName; };
    inline RtAnimAnimation* GetAnimation(void) const    { return m_pHAnimation; };
    inline float GetEndTime(void) const                 { return m_pHAnimation->duration; };

private:
    const char* m_pszName;
    RtAnimAnimation* m_pHAnimation;
};