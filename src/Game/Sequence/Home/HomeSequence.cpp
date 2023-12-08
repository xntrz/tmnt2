#include "HomeSequence.hpp"
#include "HomeTypes.hpp"
#include "HomeStageSequence.hpp"

#include "Game/Component/GameMain/GameTypes.hpp"
#include "Game/Component/GameData/GameData.hpp"
#include "Game/ProcessList.hpp"


/*static*/ CProcess* CHomeSequence::Instance(void)
{
    return new CHomeSequence;
};


CHomeSequence::CHomeSequence(void)
: m_iCallLabel(PROCESSTYPES::LABEL_EOL)
{
    ;
};


CHomeSequence::~CHomeSequence(void)
{
    ;
};


bool CHomeSequence::OnAttach(const void* pParam)
{
    return true;
};


void CHomeSequence::OnDetach(void)
{
    ;
};


void CHomeSequence::OnMove(bool bRet, const void* pReturnValue)
{
    int32 iCallParam = 0;
    
    if (bRet)
    {
        int32 iNextSequenceLabel = Branch(m_iCallLabel, pReturnValue);
        if (iNextSequenceLabel == PROCLABEL_SEQ_ENDING)
        {
            if (m_iCallLabel == PROCLABEL_SEQ_ANTIQUESHOP)
            {
                iCallParam = GAMETYPES::ENDINGTYPE_SHOP;
            }
            else if (m_iCallLabel == PROCLABEL_SEQ_AREAPLAY)
            {
                iCallParam = GAMETYPES::ENDINGTYPE_NEXUS;
            };
        }
        else if (iNextSequenceLabel == PROCESSTYPES::LABEL_EOL)
        {
            //
            //  Returning to GameMainSequence
            //
            if (m_iCallLabel == PROCLABEL_SEQ_AREAPLAY ||
                m_iCallLabel == PROCLABEL_SEQ_ENDING)
                iCallParam = PROCLABEL_SEQ_TITLE;
        };

		m_iCallLabel = iNextSequenceLabel;
    }
    else
    {
        m_iCallLabel = PROCLABEL_SEQ_HOMESTAGE;
    };

    bool bResult = false;    
    if (m_iCallLabel == PROCESSTYPES::LABEL_EOL)
    {
        if (iCallParam)
            bResult = Ret((const void*)iCallParam);
        else
            bResult = Ret();
    }
    else
    {
        if (iCallParam)
            bResult = Call(m_iCallLabel, (const void*)iCallParam);
        else
            bResult = Call(m_iCallLabel);
    };

    ASSERT(bResult != false);
};


void CHomeSequence::OnDraw(void) const
{
    ;
};


int32 CHomeSequence::Branch(int32 iLabel, const void* param)
{
    int32 iNextSequenceLabel = PROCESSTYPES::LABEL_EOL;
    
    switch (iLabel)
    {
    case PROCLABEL_SEQ_HOMESTAGE:
        {
            switch (CHomeStageSequence::m_eDoorKind)
            {
            case HOMETYPES::DOORKIND_EXIT:
                {
                    iNextSequenceLabel = PROCESSTYPES::LABEL_EOL;
                }
                break;

            case HOMETYPES::DOORKIND_APRIL:
                {
                    CGameData::Record().Antique().SetShopState(CAntiqueRecord::SHOPSTATE_NORMAL);
                    iNextSequenceLabel = PROCLABEL_SEQ_ANTIQUESHOP;
                }
                break;

            case HOMETYPES::DOORKIND_COMPUTER:
                {
                    CGameData::Record().Database().SetDatabaseState(CDatabaseRecord::DBSTATE_NORMAL);
                    iNextSequenceLabel = PROCLABEL_SEQ_DATABASESEQ;
                }
                break;

            case HOMETYPES::DOORKIND_TOURNAMENT:
                {
                    CGameData::Record().Nexus().SetNexusState(CNexusRecord::NEXUSSTATE_NORMAL);
                    CGameData::PlayParam().ClearArea();
                    iNextSequenceLabel = PROCLABEL_SEQ_NEXUSMENU;
                }
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case PROCLABEL_SEQ_DATABASESEQ:
        {
            if (CGameData::Option().Play().IsAutosaveEnabled())
                iNextSequenceLabel = PROCLABEL_SEQ_SAVELOADAUTO;
            else
                iNextSequenceLabel = PROCLABEL_SEQ_HOMESTAGE;
        }
        break;

    case PROCLABEL_SEQ_ANTIQUESHOP:
        {
            if (CGameData::Record().Antique().CheckJustCompleted())
            {
                iNextSequenceLabel = PROCLABEL_SEQ_ENDING;
            }
            else
            {
                if (CGameData::Option().Play().IsAutosaveEnabled())
                    iNextSequenceLabel = PROCLABEL_SEQ_SAVELOADAUTO;
                else
                    iNextSequenceLabel = PROCLABEL_SEQ_HOMESTAGE;
            };
        }
        break;

    case PROCLABEL_SEQ_NEXUSMENU:
        {
            if (CGameData::PlayParam().GetArea() != AREAID::ID_NONE)
            {
                iNextSequenceLabel = PROCLABEL_SEQ_AREAPLAY;
            }
            else
            {
                if (CGameData::Option().Play().IsAutosaveEnabled())
                    iNextSequenceLabel = PROCLABEL_SEQ_SAVELOADAUTO;
                else
                    iNextSequenceLabel = PROCLABEL_SEQ_HOMESTAGE;
            };
        }
        break;

    case PROCLABEL_SEQ_NEXUSRETRY:
        {
            bool bRetryFlag = bool(param != nullptr);
            
            if (bRetryFlag)
            {
                CGameData::PlayParam().ClearArea();
                CGameData::PlayParam().SetStartArea(CGameData::PlayParam().GetArea(), 0);
                iNextSequenceLabel = PROCLABEL_SEQ_AREAPLAY;
            }
            else
            {
                iNextSequenceLabel = PROCLABEL_SEQ_HOMESTAGE;
            };
        }
        break;

    case PROCLABEL_SEQ_SAVELOADAUTO:
        {
            iNextSequenceLabel = PROCLABEL_SEQ_HOMESTAGE;
        }
        break;

    case PROCLABEL_SEQ_AREAPLAY:
        {
            CGamePlayResult::AREARESULT result = CGameData::PlayResult().GetAreaResult();

            switch (result)
            {
            case CGamePlayResult::AREARESULT_GAMECLEAR:
                {
                    if (CGameData::PlayParam().GetArea() != AREAID::ID_AREA60_D)
                        iNextSequenceLabel = PROCLABEL_SEQ_HOMESTAGE;
                    else
                        iNextSequenceLabel = PROCLABEL_SEQ_ENDING;
                }
                break;

            case CGamePlayResult::AREARESULT_GAMEOVER:
                {
                    iNextSequenceLabel = PROCLABEL_SEQ_NEXUSRETRY;
                }
                break;

            case CGamePlayResult::AREARESULT_EXIT:
                {
                    CGamePlayResult::EXITSUB exitsub = CGameData::PlayResult().GetExitSub();
                    switch (exitsub)
                    {
					case CGamePlayResult::EXITSUB_TO_AREASEL:
                        {
                            CGameData::PlayParam().ClearArea();
                            iNextSequenceLabel = PROCLABEL_SEQ_NEXUSMENU;
                        }
                        break;

                    case CGamePlayResult::EXITSUB_TO_TITLE:
                        {
                            iNextSequenceLabel = PROCESSTYPES::LABEL_EOL;
                        }
                        break;

                    default:
                        {
                            ASSERT(false);
                        }
                        break;
                    };
                }
                break;

            default:
                ASSERT(false);
                break;
            };
        }
        break;

    case PROCLABEL_SEQ_ENDING:
        {
            iNextSequenceLabel = PROCESSTYPES::LABEL_EOL;
        }
        break;

    default:
        ASSERT(false);
        break;
    };

    return iNextSequenceLabel;
};