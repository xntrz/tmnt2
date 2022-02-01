#pragma once


class CMotionController;
class CModel;


class CGimmickMotion final
{
public:
    CGimmickMotion(CModel* pModel);
    ~CGimmickMotion(void);
    void Update(float dt);
    void Start(float fEndTime);
    void Stop(void);
    void AddMotion(const char* pszName);
    void SetMotion(const char* pszName, float fStartTime, float fEndTime, float fBlendTime, bool bForce);
    float GetEndTime(void) const;

private:
    CMotionController* m_pMotionController;
};