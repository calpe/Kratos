//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main authors:    Suneth Warnakulasuriya
//

#if !defined(KRATOS_K_OMEGA_SST_ELEMENT_DATA_OMEGA_ELEMENT_DATA_H_INCLUDED)
#define KRATOS_K_OMEGA_SST_ELEMENT_DATA_OMEGA_ELEMENT_DATA_H_INCLUDED

// System includes

// Project includes
#include "containers/array_1d.h"
#include "containers/variable.h"
#include "geometries/geometry_data.h"
#include "includes/node.h"
#include "includes/process_info.h"
#include "includes/ublas_interface.h"

// Application includes
#include "custom_elements/convection_diffusion_reaction_element_data.h"

namespace Kratos
{
///@name  Functions
///@{

namespace KOmegaSSTElementData
{
template <unsigned int TDim>
class OmegaElementData : public ConvectionDiffusionReactionElementData
{
public:
    using BaseType = ConvectionDiffusionReactionElementData;
    using NodeType = Node<3>;
    using GeomtryType = BaseType::GeometryType;

    static const Variable<double>& GetScalarVariable();

    static void Check(
        const GeometryType& rGeometry,
        const ProcessInfo& rCurrentProcessInfo);

    static const std::string GetName()
    {
        return "KOmegaSSTOmegaElementData";
    }

    OmegaElementData(
        const GeometryType& rGeometry,
        const Properties& rProperties,
        const ProcessInfo& rProcessInfo)
        : BaseType(rGeometry, rProperties, rProcessInfo)
    {
    }

    void CalculateConstants(
        const ProcessInfo& rCurrentProcessInfo);

    void CalculateGaussPointData(
        const Vector& rShapeFunctions,
        const Matrix& rShapeFunctionDerivatives,
        const int Step = 0);

protected:
    BoundedMatrix<double, TDim, TDim> mVelocityGradient;
    array_1d<double, 3> mTurbulentKineticEnergyGradient;
    array_1d<double, 3> mTurbulentSpecificEnergyDissipationRateGradient;

    double mBeta1;
    double mBeta2;
    double mSigmaOmega1;
    double mSigmaOmega2;
    double mBetaStar;
    double mTurbulentKineticEnergy;
    double mTurbulentSpecificEnergyDissipationRate;
    double mKinematicViscosity;
    double mWallDistance;
    double mCrossDiffusion;
    double mF1;
    double mBlendedSigmaOmega;
    double mBlendedBeta;
    double mBlendedGamma;
    double mVelocityDivergence;
    double mTurbulentKinematicViscosity;
    double mKappa;
    double mDensity;
};

///@}

} // namespace KOmegaSSTElementData

} // namespace Kratos

#endif