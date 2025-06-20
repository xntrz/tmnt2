#include "MotionParameterManager.hpp"
#include "CharacterParameter.hpp"

#include "Game/System/Hit/HitAttackData.hpp"


#ifdef _DEBUG
#define MOTION_PACKAGE_NUM (256)
#else
#define MOTION_PACKAGE_NUM (16)
#endif


class CBinaryReader final
{
public:
    CBinaryReader(void* pBuffer, uint32 uBufferSize);
    ~CBinaryReader(void);
    char* PosCurrent(void) const;
    char* PosOriginal(void) const;
    void skip(uint32 uBytes);
    
    template<class T>
    inline T read(void);

    template<class T>
    inline T peek(void);

private:
    template<class T>
    inline T buff_io(bool peek);

private:
    char* m_pBuffOrg;
    char* m_pBuffPos;
    uint32 m_uBuffSize;
};


class CCharacterParameterContainer
{
public:
    class CPackage : public CCharacterParameter, public CListNode<CPackage>
    {
    public:
        CPackage(void);
        virtual ~CPackage(void);
        void Initialize(const char* pszName, void* pBuffer, uint32 uBufferSize, int32 iGeneration);
        void ReadMotionName(CBinaryReader& br, char* pszName);
        int32 GetGeneration(void) const;
        
    private:
        int32 m_iGeneration;
    };

public:
    CCharacterParameterContainer(void);
    virtual ~CCharacterParameterContainer(void);
    void Read(const char* pszName, void* pBuffer, uint32 uBufferSize);
    CPackage* GetPackage(const char* pszName);
    CPackage* FindPackage(const char* pszName);
    CPackage* AllocPackage(void);

private:
    CPackage m_aPackage[MOTION_PACKAGE_NUM];
    CList<CPackage> m_listPackageFree;
    CList<CPackage> m_listPackageAlloc;
    int32 m_iGeneration;
};


//
// *********************************************************************************
//


CBinaryReader::CBinaryReader(void* pBuffer, uint32 uBufferSize)
: m_pBuffOrg(reinterpret_cast<char*>(pBuffer))
, m_pBuffPos(m_pBuffOrg)
, m_uBuffSize(uBufferSize)
{
    ASSERT(m_pBuffOrg);
    ASSERT(m_pBuffPos);
    ASSERT(m_uBuffSize > 0);
};


CBinaryReader::~CBinaryReader(void)
{
    ;
};


char* CBinaryReader::PosCurrent(void) const
{
    return m_pBuffPos;
};


char* CBinaryReader::PosOriginal(void) const
{
    return m_pBuffOrg;
};


void CBinaryReader::skip(uint32 uBytes)
{
    m_pBuffPos += uBytes;
};


template<class T>
T CBinaryReader::read(void)
{
    return buff_io<T>(false);
};


template<class T>
T CBinaryReader::peek(void)
{
    return buff_io<T>(true);
};


template<class T>
T CBinaryReader::buff_io(bool peek)
{
    static_assert(std::is_fundamental<T>::value, "not fundamental type");
    
    T result = *reinterpret_cast<T*>(m_pBuffPos);
    if (!peek)
        m_pBuffPos += sizeof(T);
    
    return result;
};


//
// *********************************************************************************
//


CCharacterParameterContainer::CPackage::CPackage(void)
: m_iGeneration(-1)
{
    ;
};


CCharacterParameterContainer::CPackage::~CPackage(void)
{
    ;
};


void CCharacterParameterContainer::CPackage::Initialize(const char* pszName, void* pBuffer, uint32 uBufferSize, int32 iGeneration)
{
    struct HEADER
    { 
        char MAGIC[8];    //  'TMNTCHR'
        uint32 flags;
        uint32 unknow[5];
    };
    
    enum : uint8
    {
        ID_FILE_END = 0,
        ID_CATEGORY_END = 0,

        ID_HIT_MAP,
        ID_HIT_BODY,
        ID_HIT_CATCH,
        ID_MOTION,
        ID_MOTION_NAME,
        ID_MOTION_BLEND,
        ID_PLAYMODE,
        ID_MOTION_ATTACK,
        ID_MOTION_CHAIN,
        ID_LOCUS,
        ID_MOTION_TIMING,
        ID_OFF_ATOMIC,
        ID_OFF_ATOMIC_DATA,
        ID_POSITION,
    };
    
    ASSERT(pszName);
    ASSERT(std::strlen(pszName) < CMotionParameterManager::MODEL_NAME_MAX);
    ASSERT(pBuffer);

    m_iGeneration = iGeneration;

    HEADER* pHeader = reinterpret_cast<HEADER*>(pBuffer);
    if (pHeader->flags < 7)
        m_bReverseAtomicNo = true;
    
    uint8 id = 0;
    CBinaryReader br(++pHeader, uBufferSize);

    id = br.read<uint8>();
    ASSERT(id == ID_HIT_MAP);

    //
    //  Read chara param
    //
    INIT_PARAMETER CharaParam;
    CharaParam.m_fHeight = br.read<float>();
    CharaParam.m_fRadius = br.read<float>();
    std::strcpy(CharaParam.m_szName, pszName);
    CCharacterParameter::Initialize(&CharaParam);

    //
    //  Read pos param
    //
	id = br.peek<uint8>();
    if (id == ID_POSITION)
    {
		br.read<uint8>();

        int32 nPosNum = static_cast<int32>(br.read<uint8>());
        for (int32 i = 0; i < nPosNum; ++i)
        {
            CHitSphereParameter::INIT_PARAMETER InitParam;
            InitParam.m_nBoneID         = static_cast<int32>(br.read<uint8>());
            InitParam.m_sphere.center.x = br.read<float>();
            InitParam.m_sphere.center.y = br.read<float>();
            InitParam.m_sphere.center.z = br.read<float>();
            InitParam.m_sphere.radius   = br.read<float>();

            CCharacterParameter::CreatePosition(&InitParam);

            id = br.read<uint8>();
            ASSERT(id == ID_CATEGORY_END);
        };
    };

    //
    //  Read body param
    //
    id = br.read<uint8>();
    ASSERT(id == ID_HIT_BODY);

    int32 nBodyNum = static_cast<int32>(br.read<uint8>());
    for (int32 i = 0; i < nBodyNum; ++i)
    {
        CHitSphereParameter::INIT_PARAMETER BodyParam;
        BodyParam.m_nBoneID         = static_cast<int32>(br.read<uint8>());
        BodyParam.m_sphere.center.x = br.read<float>();
        BodyParam.m_sphere.center.y = br.read<float>();
        BodyParam.m_sphere.center.z = br.read<float>();
        BodyParam.m_sphere.radius   = br.read<float>();

        CCharacterParameter::CreateHitBody(&BodyParam);

        id = br.read<uint8>();
        ASSERT(id == ID_CATEGORY_END);
    };

    //
    //  Read catch param
    //
    id = br.read<uint8>();
    ASSERT(id == ID_HIT_CATCH);

    int32 nCatchNum = static_cast<int32>(br.read<uint8>());
    for (int32 i = 0; i < nCatchNum; ++i)
    {
        CHitSphereParameter::INIT_PARAMETER CatchParam;
        CatchParam.m_nBoneID         = static_cast<int32>(br.read<uint8>());
        CatchParam.m_sphere.center.x = br.read<float>();
        CatchParam.m_sphere.center.y = br.read<float>();
        CatchParam.m_sphere.center.z = br.read<float>();
        CatchParam.m_sphere.radius   = br.read<float>();

        CCharacterParameter::CreateHitCatch(&CatchParam);

        id = br.read<uint8>();
        ASSERT(id == ID_CATEGORY_END);
    };

    //
    //  Read motion
    //
    id = br.read<uint8>();
    ASSERT(id == ID_MOTION);

    int32 nMotionNum = static_cast<int32>(br.read<uint8>());
    for (int32 i = 0; i < nMotionNum; ++i)
    {
        //
        //  Read motion init para
        //
        id = br.read<uint8>();
        ASSERT(id == ID_MOTION_NAME);

        CMotionParameter::INIT_PARAMETER MotionParam;
        MotionParam.m_szName[0] = '\0';
        MotionParam.m_fBlendTime = 0.0f;
        MotionParam.m_playmode = 0;
        ReadMotionName(br, MotionParam.m_szName);

        if (br.peek<uint8>() == ID_MOTION_BLEND)
        {
            br.read<uint8>();
            MotionParam.m_fBlendTime = br.read<float>();
        };

        id = br.read<uint8>();
        ASSERT(id == ID_PLAYMODE);

        MotionParam.m_playmode = static_cast<int32>(br.read<uint8>());
        CCharacterParameter::CreateMotion(&MotionParam);

        //
        //  Read motion attack param
        //
        if (br.peek<uint8>() == ID_MOTION_ATTACK)
        {
            br.read<uint8>();

            int32 nAttackNum = static_cast<int32>(br.read<uint8>());
            for (int32 j = 0; j < nAttackNum; ++j)
            {
                CAttackParameter::INIT_PARAMETER AttackParam;
                AttackParam.m_no              = j;
                AttackParam.m_nBoneID         = br.read<uint8>();
                AttackParam.m_sphere.center.x = br.read<float>();
                AttackParam.m_sphere.center.y = br.read<float>();
                AttackParam.m_sphere.center.z = br.read<float>();
                AttackParam.m_sphere.radius   = br.read<float>();
                AttackParam.m_fStart          = br.read<float>();
                AttackParam.m_fEnd            = br.read<float>();

                CCharacterParameter::CreateAttack(&AttackParam);

                id = br.read<uint8>();
                ASSERT(id == ID_CATEGORY_END);
            };
        };

        //
        //  Read motion chain param
        //
        if (br.peek<uint8>() == ID_MOTION_CHAIN)
        {
            br.read<uint8>();

            CChainMotionParameter::INIT_PARAMETER ChainInitParam;
            ChainInitParam.m_fStart = br.read<float>();
            ChainInitParam.m_fEnd   = br.read<float>();

            CCharacterParameter::CreateChainMotion(&ChainInitParam);

            int32 nChainNum = static_cast<int32>(br.read<uint8>());
            for (int32 j = 0; j < nChainNum; ++j)
            {
                CChainMotionParameter::ATTACH_PARAMETER ChainAttachParam;
                ChainAttachParam.m_nButton = static_cast<int32>(br.read<uint8>());
                ReadMotionName(br, ChainAttachParam.m_szMotionName);

                CCharacterParameter::AttachChainMotion(&ChainAttachParam);
            };

            id = br.read<uint8>();
            ASSERT(id == ID_CATEGORY_END);
        };

        //
        //  Read motion timings
        //
        if (br.peek<uint8>() == ID_MOTION_TIMING)
        {
            br.read<uint8>();

            int32 nTimingNum = static_cast<int32>(br.read<uint8>());
            for (int32 j = 0; j < nTimingNum; ++j)
            {
                CTimingParameter::INIT_PARAMETER timingParameter;
                timingParameter.m_kind   = static_cast<CTimingParameter::TIMING_KIND>(br.read<uint8>());
                timingParameter.m_fTime  = br.read<float>();
                timingParameter.m_nParam = static_cast<int32>(br.read<int32>());

                CCharacterParameter::CreateTiming(&timingParameter);

                id = br.read<uint8>();
                ASSERT(id == ID_CATEGORY_END);
            };
        };

        id = br.read<uint8>();
        ASSERT(id == ID_CATEGORY_END);
    };

    //
    //  Read locus para
    //
    if (br.peek<uint8>() == ID_LOCUS)
    {
		br.read<uint8>();

        int32 nLocusNum = static_cast<int32>(br.read<uint8>());
        for (int32 i = 0; i < nLocusNum; ++i)
        {
            CLocusParameter::INIT_PARAMETER LocusParam;
            LocusParam.m_nBoneID        = static_cast<int32>(br.read<uint8>());
            LocusParam.m_vPositionHigh.x= br.read<float>();
            LocusParam.m_vPositionHigh.y= br.read<float>();
            LocusParam.m_vPositionHigh.z= br.read<float>();

            LocusParam.m_Color.red      = static_cast<RwUInt8>(br.read<uint8>());
            LocusParam.m_Color.green    = static_cast<RwUInt8>(br.read<uint8>());
            LocusParam.m_Color.blue     = static_cast<RwUInt8>(br.read<uint8>());
            LocusParam.m_Color.alpha    = static_cast<RwUInt8>(br.read<uint8>());

            LocusParam.m_vPositionLow.x = br.read<float>();
            LocusParam.m_vPositionLow.y = br.read<float>();
            LocusParam.m_vPositionLow.z = br.read<float>();

            br.read<uint32>();
            
            CCharacterParameter::CreateLocus(&LocusParam);

            id = br.read<uint8>();
            ASSERT(id == ID_CATEGORY_END);
        };
    };

    //
    //  Read atomic disp param
    //
    if (br.peek<uint8>() == ID_OFF_ATOMIC)
    {
        br.read<uint8>();
        
        CCharacterParameter::SetExistOffAtomic(true);
    };

    if (br.peek<uint8>() == ID_OFF_ATOMIC_DATA)
    {
        br.read<uint8>();
        
        int32 nOffAtomic = static_cast<int32>(br.read<uint8>());
        CCharacterParameter::SetDefaultOffAtomicNum(nOffAtomic);
        
        for (int32 i = 0; i < nOffAtomic; ++i)
        {
            int32 atomicID = static_cast<int32>(br.read<uint8>());
            CCharacterParameter::SetDefaultOffAtomicNo(i, atomicID);
        };
    };

    id = br.read<uint8>();
    ASSERT(id == ID_CATEGORY_END);

    id = br.read<uint8>();
    while (id == 1)
    {
        char szMotionName[CMotionParameterManager::MOTION_NAME_MAX];
        szMotionName[0] = '\0';

        ReadMotionName(br, szMotionName);
        
        id = br.read<uint8>();
        if (id == 2)
        {
            uint32 i = 0;
            uint32 attackNo = 0;
            do
            {
                CAttackParameter::ATTACH_PARAMETER AttackAttachParam;
                AttackAttachParam.m_nNo = 0;
                AttackAttachParam.m_nPower = 0;
                AttackAttachParam.m_nStatus = 0;
                AttackAttachParam.m_fStatusVal1 = 0.0f;
                AttackAttachParam.m_fStatusVal2 = 0.0f;
                AttackAttachParam.m_bConfusion = false;
                AttackAttachParam.m_bGuardImpact = false;
                AttackAttachParam.m_bSlice = false;
                AttackAttachParam.m_bReflectShot = false;
                AttackAttachParam.m_nAntiguard = 0;
                AttackAttachParam.m_uTarget = 0;

                uint8 attackParamType = br.read<uint8>();

                if (attackParamType)
                {
                    do
                    {
                        switch (attackParamType)
                        {
                        case 3:
                            AttackAttachParam.m_nPower = br.read<int32>();
                            break;

                        case 4:
                            AttackAttachParam.m_nStatus = static_cast<int32>(br.read<uint8>());
                            break;

                        case 5:
                            AttackAttachParam.m_fStatusVal1 = br.read<float>();
                            break;

                        case 6:
                            AttackAttachParam.m_fStatusVal2 = br.read<float>();
                            break;

                        case 7:
                            AttackAttachParam.m_bConfusion = (br.read<uint8>() > 0);
                            break;

                        case 8:
							AttackAttachParam.m_nAntiguard = static_cast<int32>(br.read<uint8>());
                            break;

                        case 9:
                            AttackAttachParam.m_bGuardImpact = (br.read<uint8>() > 0);
                            break;

                        case 10:
                            AttackAttachParam.m_bSlice = (br.read<uint8>() > 0);
                            break;

                        case 11:
                            AttackAttachParam.m_uTarget |= 1;
                            break;

                        case 12:
                            AttackAttachParam.m_uTarget |= 2;
                            break;

                        case 13:
                            AttackAttachParam.m_uTarget |= 4;
                            break;

                        case 14:
                            AttackAttachParam.m_uTarget |= 8;
                            AttackAttachParam.m_bReflectShot = 1;
                            break;

                        default:
                            ASSERT(false, "unknow attack data type");
                            break;
                        };

                        attackParamType = br.read<uint8>();

                    } while (attackParamType);

                    if ((AttackAttachParam.m_nStatus != CHitAttackData::STATUS_STUN)   &&
                        (AttackAttachParam.m_nStatus != CHitAttackData::STATUS_DINDLE) &&
                        (AttackAttachParam.m_nStatus != CHitAttackData::STATUS_SLEEP)  &&
                        (AttackAttachParam.m_nStatus != CHitAttackData::STATUS_FREEZE) &&
                        (AttackAttachParam.m_nStatus != CHitAttackData::STATUS_BIND))
                    {
                        ++attackNo;
                    };
                }
                else
                {
                    ++attackNo;
                };

                AttackAttachParam.m_nNo = attackNo;

                CCharacterParameter::AttachAttack(szMotionName, i, &AttackAttachParam);
                
                id = br.read<uint8>();
                
                ++i;
            } while (id == 2);
        };
    };

    ASSERT(id == ID_FILE_END);
};


void CCharacterParameterContainer::CPackage::ReadMotionName(CBinaryReader& br, char* pszName)
{
    if (br.read<uint8>() != 0xFF)
    {
        pszName[0] = '\0';
        ASSERT(false);
    };

    uint8 len = br.read<uint8>();
    ASSERT(len <= CMotionParameterManager::MOTION_NAME_MAX);
    
    std::strncpy(pszName, br.PosCurrent(), len);
    br.skip(len);
};


int32 CCharacterParameterContainer::CPackage::GetGeneration(void) const
{
    return m_iGeneration;
};


//
// *********************************************************************************
//


CCharacterParameterContainer::CCharacterParameterContainer(void)
: m_iGeneration(0)
{
    for (int32 i = 0; i < COUNT_OF(m_aPackage); ++i)
        m_listPackageFree.push_back(&m_aPackage[i]);
};


CCharacterParameterContainer::~CCharacterParameterContainer(void)
{
    for (CPackage& it : m_listPackageAlloc)
        it.Cleanup();    
};


void CCharacterParameterContainer::Read(const char* pszName, void* pBuffer, uint32 uBufferSize)
{
    ASSERT(pszName);
    ASSERT(pBuffer);
    ASSERT(uBufferSize > 0);

    CPackage* pPackage = AllocPackage();
    ASSERT(pPackage);
    
    pPackage->Initialize(pszName, pBuffer, uBufferSize, m_iGeneration);
};


CCharacterParameterContainer::CPackage* CCharacterParameterContainer::GetPackage(const char* pszName)
{
    return FindPackage(pszName);
};


CCharacterParameterContainer::CPackage* CCharacterParameterContainer::FindPackage(const char* pszName)
{
    ASSERT(pszName);

    for (CPackage& it : m_listPackageAlloc)
    {
        if (!std::strcmp(it.GetName(), pszName))
            return &it;
    };

    return nullptr;
};


CCharacterParameterContainer::CPackage* CCharacterParameterContainer::AllocPackage(void)
{
    if (m_listPackageFree.empty())
        return nullptr;
    
    CPackage* pNode = m_listPackageFree.front();
    m_listPackageFree.erase(pNode);
    m_listPackageAlloc.push_back(pNode);

    return pNode;
};


static CCharacterParameterContainer* s_pCharacterParameterContainer = nullptr;


static inline CCharacterParameterContainer& CharacterParamContainer(void)
{
    ASSERT(s_pCharacterParameterContainer);
    return *s_pCharacterParameterContainer;
};


/*static*/ void CMotionParameterManager::Initialize(void)
{
    if (!s_pCharacterParameterContainer)
        s_pCharacterParameterContainer = new CCharacterParameterContainer;
};


/*static*/ void CMotionParameterManager::Terminate(void)
{
    if (s_pCharacterParameterContainer)
    {
        delete s_pCharacterParameterContainer;
        s_pCharacterParameterContainer = nullptr;
    };
};


/*static*/ void CMotionParameterManager::Read(const char* pszName, void* pBuffer, uint32 uBufferSize)
{
    CharacterParamContainer().Read(pszName, pBuffer, uBufferSize);
};


/*static*/ CCharacterParameter* CMotionParameterManager::GetCharacterParameter(const char* pszName, int32 nPartsNum)
{
    CCharacterParameter* pPackage = CharacterParamContainer().GetPackage(pszName);
    pPackage->SetAtomicNum(nPartsNum);
    return pPackage;
};