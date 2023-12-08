#include "Intersection.hpp"


namespace Intersection
{
    float GetSphereDistanceSQ(
        const RwV3d*    pPosP,
        float           fRadiusP,
        const RwV3d*    pPosQ,
        float           fRadiusQ
    )
    {
        ASSERT(pPosP);
        ASSERT(fRadiusP >= 0.0f);
        ASSERT(pPosQ);
        ASSERT(fRadiusQ >= 0.0f);

        RwV3d vDist = Math::VECTOR3_ZERO;        
        Math::Vec3_Sub(&vDist, pPosP, pPosQ);

        return ( Math::Vec3_Dot(&vDist, &vDist) - ((fRadiusP + fRadiusQ) * (fRadiusP + fRadiusQ)) );
    };


    bool SphereAndSphere(
        const RwV3d*    pPosP,
        float           fRadiusP,
        const RwV3d*    pPosQ,
        float           fRadiusQ
    )
    {
        ASSERT(pPosQ);
        ASSERT(fRadiusP >= 0.0f);
        ASSERT(pPosP);
        ASSERT(fRadiusQ >= 0.0f);

        float fRadiusSQ = fRadiusP + fRadiusQ;

        RwV3d vDist = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vDist, pPosP, pPosQ);
        Math::Vec3_Abs(&vDist, &vDist);

        return (GetSphereDistanceSQ(pPosP, fRadiusP, pPosQ, fRadiusQ) <= 0.0f);    
    };


    bool SphereAndSphereGetAwayVelocity(
        const RwV3d*    pPosP,
        float           fRadiusP,
        const RwV3d*    pPosQ,
        float           fRadiusQ,
        RwV3d*          pOutAwayVelocityP,
        RwV3d*          pOutAwayVelocityQ
    )
    {
        ASSERT(pPosQ);
        ASSERT(fRadiusP >= 0.0f);
        ASSERT(pPosP);
        ASSERT(fRadiusQ >= 0.0f);

        RwV3d vDist = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vDist, pPosQ, pPosP);
        float fDistSQ = Math::Vec3_Dot(&vDist, &vDist);

        if (((fRadiusP + fRadiusQ) * (fRadiusP + fRadiusQ)) < fDistSQ)
            return false;

        if (fDistSQ > 0.0f)
        {
            float fDistRoot = Math::Sqrt(fDistSQ);
            
            if (pOutAwayVelocityP)
            {
                Math::Vec3_Scale(
                    pOutAwayVelocityP,
                    &vDist,
                    ((fRadiusP + fRadiusQ) - fDistRoot) * -1.0f / (fDistRoot + fDistRoot)
                );
            };

            if (pOutAwayVelocityQ)
            {
                Math::Vec3_Scale(
                    pOutAwayVelocityQ,
                    pOutAwayVelocityP,
                    -1.0f
                );
            };
        }
        else
        {
            if (pOutAwayVelocityP)
                *pOutAwayVelocityP = { 0.0f, 0.0f, (fRadiusQ * 0.5f) };

            if (pOutAwayVelocityQ)
                *pOutAwayVelocityQ = { 0.0f, 0.0f, (fRadiusP * 0.5f) };
        };

        return true;
    };


    bool MoveSphereAndMoveSphereVelocity(
        const RwV3d*    pStartPosP,
        const RwV3d*    pVelocityP,
        float           fRadiusP,
        const RwV3d*    pStartPosQ,
        const RwV3d*    pVelocityQ,
        float           fRadiusQ,
        RwV3d*          pOutIntersectVelocityP,
        RwV3d*          pOutIntersectVelocityQ
    )
    {
        ASSERT(pStartPosP);
        ASSERT(pVelocityP);
        ASSERT(fRadiusP >= 0.0f);
        ASSERT(pStartPosQ);
        ASSERT(pVelocityQ);
        ASSERT(fRadiusQ >= 0.0f);

        RwV3d vRV = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vRV, pVelocityP, pVelocityQ);

        float fD = Math::Vec3_Dot(&vRV, &vRV);
        
        if (Math::FEqual(fD, 0.0f))
            return false;

        RwV3d vD = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vD, pStartPosP, pStartPosQ);
        
        float fRoot = (Math::Vec3_Dot(&vRV, &vD) * 2.0f) - (Math::Vec3_Dot(&vD, &vD) - ((fRadiusP + fRadiusQ) * (fRadiusQ + fRadiusP))) * fD;
        if (fRoot < 0.0f)
            return false;

        float fT = (-Math::Vec3_Dot(&vRV, &vD) - Math::Sqrt(fRoot)) / fD;
        if ((fT < 0.0f) || (fT  > 1.0f))
            return false;

        if (pOutIntersectVelocityP)
            Math::Vec3_Scale(pOutIntersectVelocityP, pVelocityP, fT);
        
        if (pOutIntersectVelocityQ)
            Math::Vec3_Scale(pOutIntersectVelocityQ, pVelocityQ, fT);
        
        return true;
    };


    bool LineAndSphere(
        RwLine*         pLine,
        RwSphere*       pSphere,
        float*          pfCenterDist
    )
    {
        ASSERT(pLine);
        ASSERT(pSphere);

        float fRadiusSQ = pSphere->radius * pSphere->radius;
        RwV3d vLength = Math::VECTOR3_ZERO;
        RwV3d v0 = Math::VECTOR3_ZERO;
        RwV3d v1 = Math::VECTOR3_ZERO;
        
        Math::Vec3_Sub(&vLength, &pLine->end, &pLine->start);
        float fLengthSQ = Math::Vec3_Dot(&vLength, &vLength);
        
        Math::Vec3_Sub(&v0, &pSphere->center, &pLine->start);
        Math::Vec3_Sub(&v1, &pSphere->center, &pLine->end);

        float v0SQ = Math::Vec3_Dot(&v0, &v0);
        float v1SQ = Math::Vec3_Dot(&v1, &v1);
        float vl0 = Math::Vec3_Dot(&v0, &vLength);

        float fResult = 0.0f;
        if (vl0 < 0.0f || Math::FEqual(fLengthSQ, 0.0f))
        {
            fResult = v0SQ;
        }
        else if (Math::Vec3_Dot(&v0, &v1) < 0.0f)
        {
            fResult = v0SQ - ((vl0 * vl0) / fLengthSQ);
        }
        else
        {
            fResult = v1SQ;
        };

        if (fResult >= fRadiusSQ)
            return false;

        if (pfCenterDist)
        {
            if (Math::FEqual(fLengthSQ, 0.0f))
                *pfCenterDist = 0.0f;
            else
                *pfCenterDist = vl0 / fLengthSQ;
        };

        return true;
    };


    bool LineVerticalAndSphere(RwLine* pLine, RwSphere* pSphere)
    {
        ASSERT(pLine);
        ASSERT(pSphere);
        ASSERT(IsLineVertical(pLine));

        float fRadiusSQ = pSphere->radius * pSphere->radius;

        RwV3d vTmp = Math::VECTOR3_ZERO;
        Math::Vec3_Sub(&vTmp, &pSphere->center, &pLine->start);

        if (fRadiusSQ < (vTmp.x * vTmp.x + vTmp.z * vTmp.z))
            return false;

        if (pLine->start.y <= pLine->end.y)
        {
            if ((pLine->end.y    < (pSphere->center.y - pSphere->radius)) ||
                (pLine->start.y  > (pSphere->center.y + pSphere->radius)))
                return false;
        }
        else
        {
            if ((pLine->start.y  < (pSphere->center.y - pSphere->radius)) ||
                (pLine->end.y    > (pSphere->center.y + pSphere->radius)))
                return false;
        };

        return true;
    };


    bool IsLineVertical(RwLine* pLine)
    {
        ASSERT(pLine);
        
        return (Math::FEqual(pLine->start.x, pLine->end.x) &&
                Math::FEqual(pLine->start.z, pLine->end.z));
    };


    void FindNearestPointOnLine(RwV3d* pPtResult, const RwV3d* pPt, const RwV3d* pPtStart, const RwV3d* pPtEnd)
    {
        RwV3d vTemp = Math::VECTOR3_ZERO;

        Math::Vec3_Sub(&vTemp, pPtEnd, pPtStart);
        float fDist =
            Math::Vec3_Dot(&vTemp, pPt) -
            Math::Vec3_Dot(&vTemp, pPtStart);

        if (fDist > 0.0f)
        {
            float fDistSQ = Math::Vec3_Dot(&vTemp, &vTemp);
            if (fDist >= fDistSQ)
            {
                *pPtResult = *pPtEnd;
            }
            else
            {
                pPtResult->x = pPtStart->x + vTemp.x * (fDist / fDistSQ);
                pPtResult->y = pPtStart->y + vTemp.y * (fDist / fDistSQ);
                pPtResult->z = pPtStart->z + vTemp.z * (fDist / fDistSQ);
            };
        }
        else
        {
            *pPtResult = *pPtStart;
        };
    };


    bool PointWithinTriangle(RwV3d* pPt, RwV3d** aTri, RwV3d* pNormal)
    {
        RwV3d p = *pPt;
        RwV3d a = *aTri[0];
        RwV3d b = *aTri[1];
        RwV3d c = *aTri[2];

        Math::Vec3_Sub(&a, &a, &p);
        Math::Vec3_Sub(&b, &b, &p);
        Math::Vec3_Sub(&c, &c, &p);

		RwV3d u = Math::VECTOR3_ZERO;
        RwV3d v = Math::VECTOR3_ZERO;
        RwV3d w = Math::VECTOR3_ZERO;

        Math::Vec3_Cross(&u, &b, &c);
        Math::Vec3_Cross(&v, &c, &a);
        Math::Vec3_Cross(&w, &a, &b);

        if (Math::Vec3_Dot(&u, &v) < 0.0f)
            return false;

        if (Math::Vec3_Dot(&u, &w) < 0.0f)
            return false;

        return true;
    };
};