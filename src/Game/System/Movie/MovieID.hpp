#pragma once


namespace MOVIEID
{
    enum VALUE
    {
        ID_START = 0,

        //
        //  Bank 0
        //
        ID_OP_TMNT1 = ID_START,
        ID_OP_TMNT2,
        ID_M01T01,
        ID_M02N01,
        ID_M02N01P,
        ID_M02N02,
        ID_M04N01,
        ID_M04N01P,
        ID_M05N01P,
        ID_M06R01,
        ID_M09N01,
        ID_M10N01,
        ID_M11J01,
        ID_M12N01,
        ID_M14N01,
        ID_M15N01,

        //
        //  Bank 1
        //
        ID_M17N01,
        ID_M18FB01,
        ID_M18FB02,
        ID_M19N01P,
        ID_M20FB01,
        ID_M20FB02,
        ID_M21N01,
        ID_M21N02,
        ID_M24N01,
        ID_M24N02,
        ID_M25J01,
        ID_M27N01,
        ID_M29N01,
        ID_M29N02,
        ID_M30NB01P,
        ID_M30NB01,

        //
        //  Bank 2
        //
        ID_M31NB01P,
        ID_M31NB01,
        ID_M32R01,
        ID_M33J01,
        ID_M38NB01,
        ID_M38NB02,
        ID_M39N01,
        ID_M40OB01,
        ID_M40OB02,
        ID_M41NB01,
        ID_M42NB01P,
        ID_M42NB01,
        ID_M43N01,
        ID_M44NB01P,
        ID_M44NB01,

        //
        //  Bank 3
        //
        ID_M45N01,
        ID_M45N02,
        ID_M46R01,
        ID_M47OB01,
        ID_M47OB02,
        ID_M48N01,
        ID_M50NB01,
        ID_M50NB01P,
        ID_M50NB02,
        ID_M52FB01P,
        ID_M52FB02P,
        ID_M53FB01P,
        ID_M53FB02P,
        ID_M54FB01P,
        ID_M54FB02P,
        ID_M55FB01P,
        ID_M55FB02P,

        //
        //  Bank 4
        //
        ID_M56NB01,
        ID_M56NB01P,
        ID_M57NB01,
        ID_M57NB02,
        ID_M58OB01,
        ID_M58OB02,
        ID_M58OB02E,
        ID_M59S01,
        ID_M59S02,
        ID_M59S03,
        ID_M60X01P,
        ID_M60X02P,
        ID_M60X03P,

        //
        //  Bank 5
        //
        ID_M60X03E,
        ID_M62X01E,

        ID_MAX,

        ID_INVALID = -1,
    };
    
    static_assert(ID_MAX == 79, "correct me");
};