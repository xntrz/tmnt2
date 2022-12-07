#pragma once


class CAIFrequencyParam
{
public:
    CAIFrequencyParam(void);
    virtual ~CAIFrequencyParam(void);
    void Initialize(uint8* pParam, int32 nParam, int32 iLevelMax);
    uint8 GetFrequency(int32 No);

private:
    int32 m_nParam;
    uint8* m_pParamTable;
};