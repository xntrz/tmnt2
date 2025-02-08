#pragma once


namespace Intersection
{
    float GetSphereDistanceSQ(const RwV3d* pPosP,
                             float fRadiusP,
                             const RwV3d* pPosQ,
                             float fRadiusQ);

    bool SphereAndSphere(const RwV3d* pPosP,
                         float fRadiusP,
                         const RwV3d* pPosQ,
                         float fRadiusQ);

    bool SphereAndSphereGetAwayVelocity(const RwV3d* pPosP,
                                        float fRadiusP,
                                        const RwV3d* pPosQ,
                                        float fRadiusQ,
                                        RwV3d* pOutAwayVelocityP = nullptr,
                                        RwV3d* pOutAwayVelocityQ = nullptr);
    
    bool MoveSphereAndMoveSphereVelocity(const RwV3d* pStartPosP,
                                         const RwV3d* pVelocityP,
                                         float fRadiusP,
                                         const RwV3d* pStartPosQ,
                                         const RwV3d* pVelocityQ,
                                         float fRadiusQ,
                                         RwV3d* pOutIntersectVelocityP = nullptr,
                                         RwV3d* pOutIntersectVelocityQ = nullptr);

    bool LineAndSphere(RwLine* pLine,
                       RwSphere* pSphere,
                       float* pfCenterDist = nullptr);

    bool LineVerticalAndSphere(RwLine* pLine, RwSphere* pSphere);

    bool IsLineVertical(RwLine* pLine);

    void FindNearestPointOnLine(RwV3d* pPtResult,
                                const RwV3d* pPt,
                                const RwV3d* pPtStart,
                                const RwV3d* pPtEnd);

    bool PointWithinTriangle(RwV3d* pPt, RwV3d** aTri, RwV3d* pNormal);
};