#pragma once


class CAIFrequencyParam
{
public:
    CAIFrequencyParam(void);
    virtual ~CAIFrequencyParam(void);
    void Initialize(uint8* pParam, int32 nParam, int32 iLevelMax);
    void DrawFreqTable(void);
    uint8 GetFrequency(int32 No) const;

private:
    int32 m_nParam;
    uint8* m_pParamTable;
};