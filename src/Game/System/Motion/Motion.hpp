#pragma once


class CMotion : public CListNode<CMotion>
{
public:
    CMotion(const char* pszName, RtAnimAnimation* pHAnimation);    
    ~CMotion(void) {};
    const char* GetName(void) const;
    RtAnimAnimation* GetAnimation(void) const;
    float GetEndTime(void) const;

private:
    const char* m_pszName;
    RtAnimAnimation* m_pHAnimation;
};