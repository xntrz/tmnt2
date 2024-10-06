#pragma once

#include "DataLoaderImpl.hpp"


namespace READERTYPE
{
    enum VALUE
    {
        MODEL = 0,
        MODEL_TOON,
        MOTION_SET,
        MOTION,
        MOTION_PARAMETER,

        TEXTURE_DICTIONARY = 7,
        MODEL_BSP,
        CAMERA,
        EFFECT,
        ANIM2D,
        UVANIM,
        FONT,
        MAP_MODEL,
        MAP_INFO,
        ADD_TEXTURE_DICTIONARY,
        GIMMICK,
        STRINGS,
        ENEMY_PARAMETER,
        ICONS_PS2,
        ICONS_NGC,

        TYPE_MAX,
    };
};


class CDataLoaderFromBinary : public IDataLoaderImpl
{
public:
    class CFileHeader
    {
    public:
        uint32 m_uMagic;
        uint32 m_uChunkNum;
    };

    class CChunkHeader
    {
    public:
        struct HEADER
        {
            uint32 m_uType;
            uint32 m_uSize;
            uint32 m_uReserved[2];
            char m_szName[16];
        };

    public:
        HEADER* m_pHeader;
        void* m_pExtraHeader;
    };

    class IItemReader
    {
    public:
        virtual ~IItemReader(void) {};
        virtual void Eval(CChunkHeader* pChunkHeader, void* pData) = 0;
        virtual const char* GetLabel(void) const = 0;
        virtual READERTYPE::VALUE GetType(void) const = 0;
    };

public:
    CDataLoaderFromBinary(void);
    virtual ~CDataLoaderFromBinary(void);
    virtual void Eval(const void* pBuffer, uint32 uBufferSize) override;
    virtual bool DecideEval(READERTYPE::VALUE ReaderType, CChunkHeader* pChunkHeader) { return true; };

private:
    IItemReader* m_apItemReader[READERTYPE::TYPE_MAX];
};