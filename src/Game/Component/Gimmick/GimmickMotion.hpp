#pragma once


class CMotionController;
class CModel;


class CGimmickMotion final
{
public:
    CGimmickMotion(CModel* pModel);
    ~CGimmickMotion(void);
    void Update(void);
    void StartRepeat(float fPlayRate);
    void StartOne(float fPlayRate);
    void Stop(void);
    void AddMotion(const char* pszName);
    void SetMotion(const char* pszName, float fStartTime, float fEndTime, float fBlendTime, bool bForce);
    void SetTime(float t);
    float GetEndTime(void) const;

    inline void SetPlayrate(float r) { m_fPlayRate = r; };

private:
    CMotionController* m_pMotionController;
    float m_fPlayRate;
};