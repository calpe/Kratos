// KRATOS  ___|  |                   |                   |
//       \___ \  __|  __| |   |  __| __| |   |  __| _` | |
//             | |   |    |   | (    |   |   | |   (   | |
//       _____/ \__|_|   \__,_|\___|\__|\__,_|_|  \__,_|_| MECHANICS
//
//  License:             BSD License
//                                       license: StructuralMechanicsApplication/license.txt
//
//  Main authors:    Vicente Mataix Ferrándiz
// 

#if !defined(KRATOS_EXACT_MORTAR_INTEGRATION_UTILITY_H_INCLUDED )
#define  KRATOS_EXACT_MORTAR_INTEGRATION_UTILITY_H_INCLUDED

// System includes
#include <iostream>

// External includes

// Project includes
#include <math.h> 

/* Includes */
#include "includes/mortar_classes.h"
#include "includes/mapping_variables.h"

// The geometry of the triangle for the "tessellation"
/* TRIANGLES */
#include "geometries/triangle_3d_3.h"
/* QUADRILATERALS */
#include "geometries/quadrilateral_3d_4.h"

// /* The integration points (we clip triangles in 3D, so with line and triangle is enought)*/
// #include "integration/line_gauss_legendre_integration_points.h"
#include "integration/triangle_gauss_legendre_integration_points.h"

/* Utilities */
#include "utilities/mortar_utilities.h"
#include "utilities/math_utils.h"

namespace Kratos
{
///@name Kratos Globals
///@{
    
///@}
///@name Type Definitions
///@{
    
    typedef Point<3>                                                                    PointType;
    typedef Node<3>                                                                      NodeType;
    typedef Geometry<NodeType>                                                   GeometryNodeType;
    typedef Geometry<PointType>                                                 GeometryPointType;
    
    ///Type definition for integration methods
    typedef GeometryData::IntegrationMethod                                     IntegrationMethod;
    typedef IntegrationPoint<2>                                              IntegrationPointType;
    typedef GeometryNodeType::IntegrationPointsArrayType                    IntegrationPointsType;
    
///@}
///@name  Enum's
///@{
    
///@}
///@name  Functions
///@{
    
///@}
///@name Kratos Classes
///@{

/** \brief ExactMortarIntegrationUtility 
 * This utility calculates the exact integration necessary for the Mortar Conditions
 */
template< unsigned int TDim, unsigned int TNumNodes>
class ExactMortarIntegrationUtility
{
public:
    ///@name Type Definitions
    ///@{
    
    typedef std::vector<array_1d<PointType,TDim>>                          ConditionArrayListType;
    
    typedef Line2D2<Point<3>>                                                            LineType;
    
    typedef Triangle3D3<Point<3>>                                                    TriangleType;
    
    typedef typename std::conditional<TDim == 2, LineType, TriangleType >::type DecompositionType;
    
    /// Pointer definition of ExactMortarIntegrationUtility
    KRATOS_CLASS_POINTER_DEFINITION(ExactMortarIntegrationUtility);
    
    ///@}
    ///@name Life Cycle
    ///@{
    
    /// Constructor
    
    /**
     * This is the default constructor
     * @param IntegrationOrder: The integration order to consider
     */
    
    ExactMortarIntegrationUtility(
        const unsigned int IntegrationOrder = 0,
        const bool DebugGeometries = false
        )
    :mIntegrationOrder(IntegrationOrder),
     mDebugGeometries(DebugGeometries)
    {
        GetIntegrationMethod();
    }
    
    /// Destructor.
    virtual ~ExactMortarIntegrationUtility() {}
    
    ///@}
    ///@name Operators
    ///@{
    
    ///@}
    ///@name Operations
    ///@{    
    
    /**
     * This utility computes the exact integration of the mortar condition (just the points, not the whole integration points)
     * @param OriginalSlaveGeometry: The geometry of the slave condition
     * @param SlaveNormal: The normal of the slave condition
     * @param OriginalMasterGeometry: The geometry of the master condition
     * @param MasterNormal: The normal of the master condition
     * @param ConditionsPointsSlave: The points that perform the exact integration
     * @return True if there is a common area (the geometries intersect), false otherwise
     */
    
    inline bool GetExactIntegration(    
        GeometryNodeType& OriginalSlaveGeometry,
        const array_1d<double, 3>& SlaveNormal,
        GeometryNodeType& OriginalMasterGeometry,
        const array_1d<double, 3>& MasterNormal,
        ConditionArrayListType& ConditionsPointsSlave
        );
    
    /**
     * This utility computes the exact integration of the mortar condition
     * @param OriginalSlaveGeometry: The geometry of the slave condition
     * @param SlaveNormal: The normal of the slave condition
     * @param OriginalMasterGeometry: The geometry of the master condition
     * @param MasterNormal: The normal of the master condition
     * @param IntegrationPointsSlave: The integrations points that belong to the slave
     * @return True if there is a common area (the geometries intersect), false otherwise
     */
    
    inline bool GetExactIntegration(    
        GeometryNodeType& OriginalSlaveGeometry,
        const array_1d<double, 3>& SlaveNormal,
        GeometryNodeType& OriginalMasterGeometry,
        const array_1d<double, 3>& MasterNormal,
        IntegrationPointsType& IntegrationPointsSlave
        )
    {
        ConditionArrayListType conditions_points_slave;
        
        const bool is_inside = GetExactIntegration(OriginalSlaveGeometry, SlaveNormal, OriginalMasterGeometry, MasterNormal, conditions_points_slave);
        
        for (unsigned int i_geom = 0; i_geom < conditions_points_slave.size(); i_geom++)
        {
            std::vector<PointType::Pointer> points_array (TDim); // The points are stored as local coordinates, we calculate the global coordinates of this points
            for (unsigned int i_node = 0; i_node < TDim; i_node++)
            {
                PointType global_point;
                OriginalSlaveGeometry.GlobalCoordinates(global_point, conditions_points_slave[i_geom][i_node]);
                points_array[i_node] = boost::make_shared<PointType>(global_point);
            }
            
            DecompositionType decomp_geom( points_array );
            
            const GeometryPointType::IntegrationPointsArrayType& local_integration_slave = decomp_geom.IntegrationPoints( mAuxIntegrationMethod );
            
            // Integrating the mortar operators
            for ( unsigned int point_number = 0; point_number < local_integration_slave.size(); point_number++ )
            {
                const double weight = local_integration_slave[point_number].Weight();
                const PointType local_point_decomp = local_integration_slave[point_number].Coordinates();
                PointType local_point_parent;
                PointType gp_global;
                decomp_geom.GlobalCoordinates(gp_global, local_point_decomp);
                OriginalSlaveGeometry.PointLocalCoordinates(local_point_parent, gp_global);
                
                const double det_J = decomp_geom.DeterminantOfJacobian( local_point_decomp ) * (TDim == 2 ? 2.0 : 1.0);
                
                IntegrationPointsSlave.push_back( IntegrationPointType( local_point_parent.Coordinate(1), local_point_parent.Coordinate(2), weight * det_J )); // TODO: Change push_back for a fic opoeration
            }
        }
        
        return is_inside;
    }
    
    /**
     * This utility computes the exact integration of the mortar condition and returns the area
     * @param OriginalSlaveGeometry: The geometry of the slave condition
     * @param SlaveNormal: The normal of the slave condition
     * @param OriginalMasterGeometry: The geometry of the master condition
     * @param MasterNormal: The normal of the master condition
     * @param Area: The total area integrated
     * @return True if there is a common area (the geometries intersect), false otherwise
     */
    
    inline bool GetExactAreaIntegration(    
        GeometryNodeType& OriginalSlaveGeometry,
        const array_1d<double, 3>& SlaveNormal,
        GeometryNodeType& OriginalMasterGeometry,
        const array_1d<double, 3>& MasterNormal,
        double& rArea
        )
    {        
        ConditionArrayListType conditions_points_slave;
        
        const bool is_inside = GetExactIntegration(OriginalSlaveGeometry, SlaveNormal, OriginalMasterGeometry, MasterNormal, conditions_points_slave);
        
        for (unsigned int i_geom = 0; i_geom < conditions_points_slave.size(); i_geom++)
        {
            std::vector<PointType::Pointer> points_array (TDim); // The points are stored as local coordinates, we calculate the global coordinates of this points
            for (unsigned int i_node = 0; i_node < TDim; i_node++)
            {
                PointType global_point;
                OriginalSlaveGeometry.GlobalCoordinates(global_point, conditions_points_slave[i_geom][i_node]);
                points_array[i_node] = boost::make_shared<PointType>(global_point);
            }
            
            DecompositionType decomp_geom( points_array );
            
            if (mDebugGeometries == true)
            {
                std::cout << "\nGraphics3D[{Opacity[.3],Triangle[{{" << decomp_geom[0].X() << "," << decomp_geom[0].Y() << "," << decomp_geom[0].Z()  << "},{" << decomp_geom[1].X() << "," << decomp_geom[1].Y() << "," << decomp_geom[1].Z()  << "},{" << decomp_geom[2].X() << "," << decomp_geom[2].Y() << "," << decomp_geom[2].Z()  << "}}]}],";// << std::endl;
            }
            
            rArea += decomp_geom.Area();
        }
        
        return is_inside;
    }
    
    /**
     * This utility computes the exact integration of the mortar condition
     * @param SlaveCond: The slave condition
     * @param MasterCond: The master condition
     * @param CustomSolution: The matrix containing the integrations points that belong to the slave
     * @return True if there is a common area (the geometries intersect), false otherwise
     */
    
    bool TestGetExactIntegration(     
        Condition::Pointer& SlaveCond,
        Condition::Pointer& MasterCond,
        Matrix& CustomSolution
        )
    {
        IntegrationPointsType integration_points_slave;
        
        const bool solution = GetExactIntegration(SlaveCond->GetGeometry(), SlaveCond->GetValue(NORMAL), MasterCond->GetGeometry(), MasterCond->GetValue(NORMAL), integration_points_slave);
        
        CustomSolution.resize(integration_points_slave.size(), TDim, false);
        
        if (mDebugGeometries == true)
        {
            std::cout << "The Gauss Points obtained are: " << std::endl;
        }
        for (unsigned int GP = 0; GP < integration_points_slave.size(); GP++)
        {
            if (mDebugGeometries == true)
            {
                KRATOS_WATCH(integration_points_slave[GP]);
            }
            
            // Solution save:
            CustomSolution(GP, 0) = integration_points_slave[GP].Coordinate(1);
            if (TDim == 2)
            {
                CustomSolution(GP, 1) = integration_points_slave[GP].Weight();
            }
            else
            {
                CustomSolution(GP, 1) = integration_points_slave[GP].Coordinate(2);
                CustomSolution(GP, 2) = integration_points_slave[GP].Weight();
            }
        }
        
        return solution;
    }
    
    /**
     * This utility computes the exact integration of the mortar condition and returns the area
     * @param SlaveCond: The slave condition
     * @return The total area integrated
     */
    
    double TestGetExactAreaIntegration(Condition::Pointer& SlaveCond)
    {
        // Initalize values
        double area = 0.0;
        boost::shared_ptr<ConditionMap>& all_conditions_maps = SlaveCond->GetValue( MAPPING_PAIRS );
        
        if (mDebugGeometries == true)
        {
//             std::cout << "\n\nID: " << SlaveCond->Id() << std::endl;
            std::cout << "\nGraphics3D[{EdgeForm[{Thick,Dashed,Red}],FaceForm[],Triangle[{{" << SlaveCond->GetGeometry()[0].X() << "," << SlaveCond->GetGeometry()[0].Y() << "," << SlaveCond->GetGeometry()[0].Z()  << "},{" << SlaveCond->GetGeometry()[1].X() << "," << SlaveCond->GetGeometry()[1].Y() << "," << SlaveCond->GetGeometry()[1].Z()  << "},{" << SlaveCond->GetGeometry()[2].X() << "," << SlaveCond->GetGeometry()[2].Y() << "," << SlaveCond->GetGeometry()[2].Z()  << "}}],Text[Style["<< SlaveCond->Id() <<", Tiny],{"<< SlaveCond->GetGeometry().Center().X() << "," << SlaveCond->GetGeometry().Center().Y() << ","<< SlaveCond->GetGeometry().Center().Z() << "}]}],";// << std::endl;
        }
        
        for (auto it_pair = all_conditions_maps->begin(); it_pair != all_conditions_maps->end(); ++it_pair )
        {
            if (mDebugGeometries == true)
            {
//                 std::cout << "\n\nID MASTER: " << (it_pair->first)->Id() << std::endl;
                if ((it_pair->first)->Is(VISITED) == false || (it_pair->first)->IsDefined(VISITED) == false)
                {
                    std::cout << "\nGraphics3D[{EdgeForm[{Thick,Dashed,Blue}],FaceForm[],Triangle[{{" << (it_pair->first)->GetGeometry()[0].X() << "," << (it_pair->first)->GetGeometry()[0].Y() << "," << (it_pair->first)->GetGeometry()[0].Z()  << "},{" << (it_pair->first)->GetGeometry()[1].X() << "," << (it_pair->first)->GetGeometry()[1].Y() << "," << (it_pair->first)->GetGeometry()[1].Z()  << "},{" << (it_pair->first)->GetGeometry()[2].X() << "," << (it_pair->first)->GetGeometry()[2].Y() << "," << (it_pair->first)->GetGeometry()[2].Z()  << "}}],Text[Style["<< (it_pair->first)->Id() <<", Tiny],{"<< (it_pair->first)->GetGeometry().Center().X() << "," << (it_pair->first)->GetGeometry().Center().Y() << ","<< (it_pair->first)->GetGeometry().Center().Z() << "}]}],";// << std::endl;
                    
                    (it_pair->first)->Set(VISITED, true);
                }
            }
            
            GetExactAreaIntegration(SlaveCond->GetGeometry(), SlaveCond->GetValue(NORMAL), (it_pair->first)->GetGeometry(), (it_pair->first)->GetValue(NORMAL), area);
        }
        
        return area;
    }
    
protected:
    ///@name Protected static Member Variables
    ///@{

    ///@}
    ///@name Protected member Variables
    ///@{

    ///@}
    ///@name Protected Operators
    ///@{

    ///@}
    ///@name Protected Operations
    ///@{

    /**
     * Get the integration method to consider
     */
        
    void GetIntegrationMethod()
    {
        // Setting the auxiliar integration points
        if (mIntegrationOrder == 1)
        {
            mAuxIntegrationMethod = GeometryData::GI_GAUSS_1;
        }
        else if (mIntegrationOrder == 2)
        {
            mAuxIntegrationMethod = GeometryData::GI_GAUSS_2;
        }
        else if (mIntegrationOrder == 3)
        {
            mAuxIntegrationMethod = GeometryData::GI_GAUSS_3;
        }
        else if (mIntegrationOrder == 4)
        {
            mAuxIntegrationMethod = GeometryData::GI_GAUSS_4;
        }
        else if (mIntegrationOrder == 5)
        {
            mAuxIntegrationMethod = GeometryData::GI_GAUSS_5;
        }
        else
        {
            mAuxIntegrationMethod = GeometryData::GI_GAUSS_2;
        }
    }
    
    /**
     * Get the integration method to consider
     */
        
    GeometryNodeType::IntegrationPointsArrayType GetIntegrationTriangle()
    {
        // Setting the auxiliar integration points
        if (mIntegrationOrder == 1)
        {
            return Quadrature<TriangleGaussLegendreIntegrationPoints1, 2, IntegrationPoint<3> >::GenerateIntegrationPoints();
        }
        else if (mIntegrationOrder == 2)
        {
            return Quadrature<TriangleGaussLegendreIntegrationPoints2, 2, IntegrationPoint<3> >::GenerateIntegrationPoints();
        }
        else if (mIntegrationOrder == 3)
        {
            return Quadrature<TriangleGaussLegendreIntegrationPoints3, 2, IntegrationPoint<3> >::GenerateIntegrationPoints();
        }
        else if (mIntegrationOrder == 4)
        {
            return Quadrature<TriangleGaussLegendreIntegrationPoints4, 2, IntegrationPoint<3> >::GenerateIntegrationPoints();
        }
        else if (mIntegrationOrder == 5)
        {
            return Quadrature<TriangleGaussLegendreIntegrationPoints5, 2, IntegrationPoint<3> >::GenerateIntegrationPoints();
        }
        else
        {
            return Quadrature<TriangleGaussLegendreIntegrationPoints2, 2, IntegrationPoint<3> >::GenerateIntegrationPoints();
        }
    }
    
    /**
     * This function intersects two lines in a 2D plane
     * @param PointOrig: The points from the origin geometry
     * @param PointDest: The points in the destination geometry
     * @return PointIntersection: The intersection point if there is any
     * @return True if there is a intersection point, false otherwise
     */
    
    static inline bool Clipping2D(
        PointType& PointIntersection, 
        const PointType PointOrig1,
        const PointType PointOrig2,
        const PointType PointDest1,
        const PointType PointDest2
        )
    {
        const double s_orig1_orig2_x = PointOrig2.Coordinate(1) - PointOrig1.Coordinate(1);
        const double s_orig1_orig2_y = PointOrig2.Coordinate(2) - PointOrig1.Coordinate(2);
        const double s_dest1_dest2_x = PointDest2.Coordinate(1) - PointDest1.Coordinate(1);
        const double s_dest1_dest2_y = PointDest2.Coordinate(2) - PointDest1.Coordinate(2);
        
        const double denom = s_orig1_orig2_x * s_dest1_dest2_y - s_dest1_dest2_x * s_orig1_orig2_y;
    
        const double tolerance = 1.0e-12; // std::numeric_limits<double>::epsilon();
        if (std::abs(denom) < tolerance) // NOTE: Collinear
        {
            return false;
        }
        
        const double s_orig1_dest1_x = PointOrig1.Coordinate(1) - PointDest1.Coordinate(1);
        const double s_orig1_dest1_y = PointOrig1.Coordinate(2) - PointDest1.Coordinate(2);
        
        const double s = (s_orig1_orig2_x * s_orig1_dest1_y - s_orig1_orig2_y * s_orig1_dest1_x)/denom;
        
        const double t = (s_dest1_dest2_x * s_orig1_dest1_y - s_dest1_dest2_y * s_orig1_dest1_x)/denom;
        
        if (s >= -tolerance && s <= (1.0 + tolerance) && t >= -tolerance && t <= (1.0 + tolerance))
        {
            PointIntersection.Coordinate(1) = PointOrig1.Coordinate(1) + t * s_orig1_orig2_x; 
            PointIntersection.Coordinate(2) = PointOrig1.Coordinate(2) + t * s_orig1_orig2_y; 
            
            return true;
        }
        else
        {
            return false;
        }
    }
    
    /**
     * This function calculates in 2D the normal vector to a given one
     * @param v: The vector to compute the normal 
     * @return n: The normal vector
     */
    
    static inline array_1d<double, 3> GetNormalVector2D(const array_1d<double, 3> v)
    {
        array_1d<double, 3> n;

        n[0] = - v[1];
        n[1] =   v[0];
        n[2] =    0.0;
    
        return n;
    }
    
    /**
     * This function calculates in 2D the angle between two points
     * @param PointOrig1: The points from the origin geometry
     * @param PointOrig2: The points in the destination geometry
     * @param Axis1: The axis respect the angle is calculated
     * @param Axis2: The normal to the previous axis
     * @return angle: The angle formed
     */
    
    static inline double AnglePoints(
        const PointType PointOrig1,
        const PointType PointOrig2,
        const array_1d<double, 3> Axis1,
        const array_1d<double, 3> Axis2
        )
    {
        array_1d<double, 3> local_edge = PointOrig2.Coordinates() - PointOrig1.Coordinates();
        if (norm_2(local_edge) > 0.0)
        {
            local_edge /= norm_2(local_edge);
        }
        
        const double xi  = inner_prod(Axis1, local_edge);
        const double eta = inner_prod(Axis2, local_edge);
        
        return (std::atan2(eta, xi));
    }

    /**
     * This function checks if two points are the same one
     * @param PointOrig: The points from the origin geometry
     * @param PointDest: The points in the destination geometry
     * @return check: The check done
     */
    
    static inline bool CheckPoints(
        const PointType PointOrig1,
        const PointType PointOrig2
        )
    {
        const double tolerance = std::numeric_limits<double>::epsilon();
        
        return (norm_2(PointOrig2.Coordinates() - PointOrig1.Coordinates()) < tolerance) ? true : false;
    }
    
    /**
     * This functions calculates the determinant of a 2D triangle (using points) to check if invert the order
     * @param PointOrig1: First point
     * @param PointOrig2: Second point
     * @param PointOrig3: Third point
     * @return The DetJ
     */
    
    static inline double FasTriagleCheck2D(
        const PointType PointOrig1,
        const PointType PointOrig2,
        const PointType PointOrig3
        )
    {
        const double x10 = PointOrig2.X() - PointOrig1.X();
        const double y10 = PointOrig2.Y() - PointOrig1.Y();

        const double x20 = PointOrig3.X() - PointOrig1.X();
        const double y20 = PointOrig3.Y() - PointOrig1.Y();

        //Jacobian is calculated:
        //  |dx/dxi  dx/deta|	|x1-x0   x2-x0|
        //J=|	        |=	|	      |
        //  |dy/dxi  dy/deta|	|y1-y0   y2-y0|
        
        return x10 * y20 - y10 * x20;
    }
    
    /**
     * This functions calculates the determinant of a 3D triangle (using points) to check if invert the order
     * @param PointOrig1: First point
     * @param PointOrig2: Second point
     * @param PointOrig3: Third point
     * @return The DetJ
     */
    
    static inline double FastTriangleCheck(
        const PointType PointOrig1,
        const PointType PointOrig2,
        const PointType PointOrig3
        )
    {
        Matrix jacobian( 3, 2 );
        jacobian( 0, 0 ) = -( PointOrig1.X() ) + ( PointOrig2.X() ); //on the Gauss points (J is constant at each element)
        jacobian( 1, 0 ) = -( PointOrig1.Y() ) + ( PointOrig2.Y() );
        jacobian( 2, 0 ) = -( PointOrig1.Z() ) + ( PointOrig2.Z() );
        jacobian( 0, 1 ) = -( PointOrig1.X() ) + ( PointOrig3.X() );
        jacobian( 1, 1 ) = -( PointOrig1.Y() ) + ( PointOrig3.Y() );
        jacobian( 2, 1 ) = -( PointOrig1.Z() ) + ( PointOrig3.Z() );
        
        const double det_j = MathUtils<double>::GeneralizedDet(jacobian);
        
//         if (std::abs(det_j) < std::numeric_limits<double>::epsilon())
//         {
//             KRATOS_ERROR << "WARNING:: Your triangle has zero area!!!!!" << std::endl; 
//         }
        
        return det_j;
    }

    /**
     * This function push backs the points that are inside
     * @param PointList: The intersection points
     * @param AllInside: The nodes that are already known as inside the other geometry
     * @param ThisGeometry: The geometry considered
     */
    
    inline void PushBackPoints(
        std::vector<PointType>& PointList,
        const array_1d<bool, TNumNodes>& AllInside,
        GeometryPointType& ThisGeometry
        )
    {
        for (unsigned int i_node = 0; i_node < TNumNodes; i_node++)
        {
            if (AllInside[i_node] == true)
            {
                // We check if the node already exists
                bool add_point = true;
                for (unsigned int iter = 0; iter < PointList.size(); iter++)
                {
                    if (CheckPoints(ThisGeometry[i_node], PointList[iter]) == true)
                    {
                        add_point = false;
                    }
                }
                        
                if (add_point == true) 
                {
                    PointList.push_back(ThisGeometry[i_node]);
                }
            }
        }
    }
    
    /**
     * This function checks if the points of Geometry2 are inside Geometry1
     * @param AllInside: The nodes that are inside or not the geometry
     * @param Geometry1: The geometry where the points are checked
     * @param Geometry2: The geometry to check
     */
    
    inline void CheckInside(
        array_1d<bool, TNumNodes>& AllInside,
        GeometryPointType& Geometry1,
        GeometryPointType& Geometry2,
        const double& Tolerance
        )
    {
        for (unsigned int i_node = 0; i_node < TNumNodes; i_node++)
        {
            GeometryNodeType::CoordinatesArrayType projected_gp_local;
        
            AllInside[i_node] = Geometry1.IsInside( Geometry2[i_node].Coordinates( ), projected_gp_local, Tolerance) ;
        }
    }
    
    /**
     * This function calculates the triangles intersections (this is a module, that can be used directly in the respective function)
     * @param ConditionsPointsSlave: The final solution vector, containing all the nodes
     * @param PointList: The intersection points
     * @param Geometry1/Geometry2: The geometries studied (projected)
     * @param SlaveTangentXi/SlaveTangentEta: The vectors used as base to rotate
     * @param RefCenter: The reference point to rotate
     * @param IsAllInside: To simplify and consider the point_list directly
     * @return If there is intersection or not (true/false)
     */
    
    inline bool TriangleIntersections(
        ConditionArrayListType& ConditionsPointsSlave,
        std::vector<PointType>& PointList,
        GeometryNodeType& OriginalSlaveGeometry,
        GeometryNodeType& OriginalMasterGeometry,
        GeometryPointType& Geometry1,
        GeometryPointType& Geometry2,
        const array_1d<double, 3>& SlaveTangentXi,
        const array_1d<double, 3>& SlaveTangentEta,
        const PointType& RefCenter,
        const bool IsAllInside = false
        )
    {   
//         if (mDebugGeometries == true)
//         { 
//             if (TNumNodes == 3)
//             {
//                 // Debug
//                 std::cout << "\nGraphics[{EdgeForm[{Thick,Dashed,Red}],FaceForm[],Triangle[{{" << Geometry1[0].X() << "," << Geometry1[0].Y() << "},{" << Geometry1[1].X() << "," << Geometry1[1].Y() << "},{" << Geometry1[2].X() << "," << Geometry1[2].Y() << "}}]}],";// << std::endl;
//                 std::cout << "\nGraphics[{EdgeForm[{Thick,Dashed,Blue}],FaceForm[],Triangle[{{" << Geometry2[0].X() << "," << Geometry2[0].Y() << "},{" << Geometry2[1].X() << "," << Geometry2[1].Y() << "},{" << Geometry2[2].X() << "," << Geometry2[2].Y() << "}}]}],";// << std::endl;
//             }
//             else if (TNumNodes == 4)
//             {
//                 // Debug
//                 std::cout << "\nGraphics[{EdgeForm[{Thick,Dashed,Red}],FaceForm[],Polygon[{{" << Geometry1[0].X() << "," << Geometry1[0].Y() << "},{" << Geometry1[1].X() << "," << Geometry1[1].Y()  << "},{" << Geometry1[2].X() << "," << Geometry1[2].Y() << "},{" << Geometry1[3].X() << "," << Geometry1[3].Y() << "}}]}],";// << std::endl;
//                 std::cout << "\nGraphics[{EdgeForm[{Thick,Dashed,Blue}],FaceForm[],Polygon[{{" << Geometry2[0].X() << "," << Geometry2[0].Y() << "},{" << Geometry2[1].X() << "," << Geometry2[1].Y() << "},{" << Geometry2[2].X() << "," << Geometry2[2].Y() << "},{" << Geometry2[3].X() << "," << Geometry2[3].Y() << "}}]}],";// << std::endl;
//             }
//         }
        
        if (IsAllInside == false)
        {
            // We consider the Z coordinate constant
            const double z_ref = RefCenter.Coordinate(3);
            
            // We find the intersection in each side
            for (unsigned int i_edge = 0; i_edge < TNumNodes; i_edge++)
            {  
                const unsigned int ip_edge = (i_edge == (TNumNodes - 1)) ? 0 : i_edge + 1;
                for (unsigned int j_edge = 0; j_edge < TNumNodes; j_edge++)
                {
                    const unsigned int jp_edge = (j_edge == (TNumNodes - 1)) ? 0 : j_edge + 1;
                    
                    PointType intersected_point;
                    const bool intersected = Clipping2D(
                        intersected_point,
                        Geometry1[i_edge],
                        Geometry1[ip_edge],
                        Geometry2[j_edge],
                        Geometry2[jp_edge]
                        );
                    
                    if (intersected == true)
                    {
                        // Set the coordinate
                        intersected_point.Coordinate(3) = z_ref;
                        
                        // Ititialize the check
                        bool add_point = true;
                        for (unsigned int iter = 0; iter < PointList.size(); iter++)
                        {
                            if (CheckPoints(intersected_point, PointList[iter]) == true)
                            {
                                add_point = false;
                                break;
                            }
                        }
                        
                        if (add_point == true) 
                        {
                            PointList.push_back(intersected_point);
                        }
                    }
                }
            }
        }
        
        // We compose the triangles 
        const unsigned int list_size = PointList.size();
        if (list_size > 2) // Technically the minimum is three, just in case I consider 2
        {
            // We reorder the nodes according with the angle they form with the first node
            std::vector<double> angles (list_size - 1);
            array_1d<double, 3> v = PointList[1].Coordinates() - PointList[0].Coordinates();
            v /= norm_2(v);
            array_1d<double, 3> n = GetNormalVector2D(v);
            
            for (unsigned int elem = 1; elem < list_size; elem++)
            {
                angles[elem - 1] = AnglePoints(PointList[0], PointList[elem], v, n);
                if (angles[elem - 1] < 0.0)
                {
                    v = PointList[elem].Coordinates() - PointList[0].Coordinates();
                    v /= norm_2(v);
                    n = GetNormalVector2D(v);
                    for (unsigned int aux_elem = 0; aux_elem <= (elem - 1); aux_elem++)
                    {
                        angles[aux_elem] -= angles[elem - 1];
                    }
                }
            }
            
            const std::vector<std::size_t> index_vector = MortarUtilities::SortIndexes<double>(angles);

            ConditionsPointsSlave.resize((list_size - 2));
            
//             if (mDebugGeometries == true)
//             {                    
//                 for (unsigned int elem = 0; elem < list_size - 2; elem++)
//                 { 
//                     std::cout << "\nGraphics[{Opacity[.3],Triangle[{{" << PointList[0].X() << "," << PointList[0].Y() << "},{" << PointList[index_vector[elem + 0] + 1].X() << "," << PointList[index_vector[elem + 0] + 1].Y() << "},{" << PointList[index_vector[elem + 1] + 1].X() << "," << PointList[index_vector[elem + 1] + 1].Y() << "}}]}],Graphics[{PointSize[Large],Point[{{" << PointList[0].X() << "," << PointList[0].Y() << "},{" << PointList[index_vector[elem + 0] + 1].X() << "," << PointList[index_vector[elem + 0] + 1].Y() << "},{" << PointList[index_vector[elem + 1] + 1].X() << "," << PointList[index_vector[elem + 1] + 1].Y() << "}}]}],";// << std::endl;
//                 }
//             }
            
            // We recover this point to the triangle plane
            for (unsigned int i_point_list = 0; i_point_list < PointList.size(); i_point_list++)
            {
                MortarUtilities::RotatePoint(PointList[i_point_list], RefCenter, SlaveTangentXi, SlaveTangentEta, true);
            }
            
            for (unsigned int elem = 0; elem < list_size - 2; elem++) // NOTE: We always have two points less that the number of nodes
            {
                array_1d<PointType, 3> points_locals;
                    
                // Now we project to the slave surface
                PointType point_local;
                
                if (FastTriangleCheck(PointList[0], PointList[index_vector[elem] + 1], PointList[index_vector[elem + 1] + 1]) > 0.0)
                {
                    OriginalSlaveGeometry.PointLocalCoordinates(point_local, PointList[0]);
                    points_locals[0] = point_local;
                    
                    OriginalSlaveGeometry.PointLocalCoordinates(point_local, PointList[index_vector[elem + 0] + 1]);
                    points_locals[1] = point_local;
                    
                    OriginalSlaveGeometry.PointLocalCoordinates(point_local, PointList[index_vector[elem + 1] + 1]);
                    points_locals[2] = point_local;
                }
                else
                {
                    OriginalSlaveGeometry.PointLocalCoordinates(point_local, PointList[index_vector[elem + 1] + 1]);
                    points_locals[0] = point_local;

                    OriginalSlaveGeometry.PointLocalCoordinates(point_local, PointList[index_vector[elem + 0] + 1]);
                    points_locals[1] = point_local;

                    OriginalSlaveGeometry.PointLocalCoordinates(point_local, PointList[0]);
                    points_locals[2] = point_local;
                }
                
                ConditionsPointsSlave[elem] = points_locals;
            }
            
            if (ConditionsPointsSlave.size() > 0)
            {                    
                return true;
            }
            else
            {
                return false;
            }
        }
//         else if(list_size == 1 || list_size == 2) // NOTE: Activate in case of unknown behaviour
//         {
//             unsigned int aux_sum = 0;
//             for (unsigned int isum = 0; isum < AllInside.size(); isum++)
//             {
//                 aux_sum += AllInside[isum];
//             }
//             
//             if (aux_sum == list_size) // NOTE: One or two can be due to concident nodes on the edges
//             {
//                 ConditionsPointsSlave.clear();
//                 return false;
//             }
//             else
//             {
//                 if (mDebugGeometries == true)
//                 {
//                     // Debug
//                     KRATOS_WATCH(Geometry1);
//                     KRATOS_WATCH(Geometry2);
//                     for (unsigned int ipoint = 0; ipoint < list_size; ipoint++)
//                     {
//                         KRATOS_WATCH(PointList[ipoint]);
//                     }
//                     
//                     // Debug (Mathematica plot!!!)
//                     for (unsigned int isum = 0; isum < AllInside.size(); isum++)
//                     {
//                         KRATOS_WATCH(AllInside[isum]);
//                     }
//                     
//                     PointType aux1;
//                     aux1.Coordinates() = Geometry1[0].Coordinates();
//                     
//                     PointType aux2;
//                     aux2.Coordinates() = Geometry1[1].Coordinates();
//                     
//                     PointType aux3;
//                     aux3.Coordinates() = Geometry1[2].Coordinates();
//                     
//                     PointType aux4;
//                     aux4.Coordinates() = Geometry2[0].Coordinates();
//                     
//                     PointType aux5;
//                     aux5.Coordinates() = Geometry2[1].Coordinates();
//                     
//                     PointType aux6;
//                     aux6.Coordinates() = Geometry2[2].Coordinates();
//                     
//                     std::cout << "Show[Graphics[{EdgeForm[Thick], Red ,Triangle[{{" << aux1.X() << "," << aux1.Y() << "},{" << aux2.X() << "," << aux2.Y() << "},{" << aux3.X() << "," << aux3.Y() << "}}]}],Graphics[{EdgeForm[Thick], Blue ,Triangle[{{" << aux4.X() << "," << aux4.Y() << "},{" << aux5.X() << "," << aux5.Y() << "},{" << aux6.X() << "," << aux6.Y() << "}}]}]";
//                     
//                     for (unsigned int ipoint = 0; ipoint < list_size; ipoint++)
//                     {
//                         std::cout << ",Graphics[{PointSize[Large],Point[{" << PointList[ipoint].X() << "," << PointList[ipoint].Y() << "}]}]";
//                     }
//                         
//                     std::cout << "]" << std::endl;
//                     
//                     std::cout << "WARNING: THIS IS NOT SUPPOSED TO HAPPEN (check if it is the edge)" << std::endl; 
//     //                 KRATOS_ERROR << "WARNING: THIS IS NOT SUPPOSED TO HAPPEN" << std::endl; 
//                 }
//             }
//         }
        else // No intersection
        {
            ConditionsPointsSlave.clear();
            return false;
        }
        
        ConditionsPointsSlave.clear();
        return false;
    }
    
    ///@}
    ///@name Protected  Access
    ///@{

    ///@}
    ///@name Protected Inquiry
    ///@{

    ///@}
    ///@name Protected LifeCycle
    ///@{
    ///@}
private:
    ///@name Static Member Variables
    ///@{
    ///@}
    ///@name Member Variables
    ///@{

    const unsigned int mIntegrationOrder;    // The integration order to consider
    IntegrationMethod mAuxIntegrationMethod; // The auxiliar list of Gauss Points taken from the geometry
    
    // NOTE: Just for debug
    const bool mDebugGeometries;             // If the geometry is debugged or not
    
    ///@}
    ///@name Private Operators
    ///@{

    ///@}
    ///@name Private Operations
    ///@{

    ///@}
    ///@name Private  Access
    ///@{
    ///@}

    ///@}
    ///@name Serialization
    ///@{

    ///@name Private Inquiry
    ///@{
    ///@}

    ///@name Unaccessible methods
    ///@{
    ///@}
}; // Class ExactMortarIntegrationUtility

// TODO: Move this to a cpp file
///@name Explicit Specializations
///@{

    template<>  
    inline bool ExactMortarIntegrationUtility<2,2>::GetExactIntegration(         
        GeometryNodeType& OriginalSlaveGeometry,
        const array_1d<double, 3>& SlaveNormal,
        GeometryNodeType& OriginalMasterGeometry,
        const array_1d<double, 3>& MasterNormal,
        ConditionArrayListType& ConditionsPointsSlave
        )
    {
        // We take the geometry GP from the core 
        const double tolerance = 1.0e-6; // std::numeric_limits<double>::epsilon();
        
        double total_weight = 0.0;
        array_1d<double,2> auxiliar_coordinates = ZeroVector(2);
        
        // Declaring auxiliar values
        PointType projected_gp_global;
        GeometryNodeType::CoordinatesArrayType projected_gp_local;
        
        // First look if the edges of the slave are inside of the master, if not check if the opposite is true, if not then the element is not in contact
        for (unsigned int i_slave = 0; i_slave < 2; i_slave++)
        {
            const array_1d<double, 3> normal = OriginalSlaveGeometry[i_slave].GetValue(NORMAL);
            
            MortarUtilities::FastProjectDirection(OriginalMasterGeometry, OriginalSlaveGeometry[i_slave].Coordinates(), projected_gp_global, MasterNormal, -normal ); // The opposite direction
            
            const bool is_inside = OriginalMasterGeometry.IsInside( projected_gp_global.Coordinates( ), projected_gp_local, tolerance );
            
            if (is_inside == true) 
            {
                if (i_slave == 0)
                {
                    auxiliar_coordinates[0] = - 1.0;
                }
                else if (i_slave == 1)
                {
                    auxiliar_coordinates[1] =   1.0;
                }
            }
        }
        
        if ((auxiliar_coordinates[0] == - 1.0 && auxiliar_coordinates[1] == 1.0) == true)
        {
            total_weight = 2.0;
        }
        else
        {
            std::vector<double> auxiliar_xi;
            for (unsigned int i_master = 0; i_master < 2; i_master++)
            {
                projected_gp_local[0] = (i_master == 0) ? -1.0 : 1.0;
                double delta_xi = (i_master == 0) ? 0.5 : -0.5;
                const bool is_inside = MortarUtilities::ProjectIterativeLine2D(OriginalSlaveGeometry, OriginalMasterGeometry[i_master].Coordinates(), projected_gp_local, SlaveNormal, tolerance, delta_xi);
                
                if (is_inside == true)
                {
                    auxiliar_xi.push_back(projected_gp_local[0]);
                }
            }
            
            if (auxiliar_xi.size() == 1 && ((auxiliar_coordinates[0] == - 1.0 || auxiliar_coordinates[1] == 1.0)))
            {
                if (std::abs(auxiliar_coordinates[0] + 1.0) < tolerance) // NOTE: Equivalent to == -1.0
                {
                    auxiliar_coordinates[1] = auxiliar_xi[0];
                }
                else if (std::abs(auxiliar_coordinates[1] - 1.0) < tolerance) // NOTE: Equivalent to == 1.0
                {
                    auxiliar_coordinates[0] = auxiliar_xi[0];
                }
                else
                {
                    KRATOS_WATCH(auxiliar_xi[0]);
                    KRATOS_WATCH(auxiliar_coordinates[0]);
                    KRATOS_WATCH(auxiliar_coordinates[1]);
                    KRATOS_ERROR << "WARNING: THIS IS NOT SUPPOSED TO HAPPEN!!!! (TYPE 0)" << std::endl;
                }
            }
            else  if (auxiliar_xi.size() == 2)
            {
                if (std::abs(auxiliar_coordinates[0] + 1.0) < tolerance) // NOTE: Equivalent to == -1.0
                {
                    auxiliar_coordinates[1] = auxiliar_xi[0] < auxiliar_xi[1] ? auxiliar_xi[1] : auxiliar_xi[0];
                }
                else if (std::abs(auxiliar_coordinates[1] - 1.0) < tolerance) // NOTE: Equivalent to == 1.0
                {
                    auxiliar_coordinates[0] = auxiliar_xi[0] < auxiliar_xi[1] ? auxiliar_xi[0] : auxiliar_xi[1];
                }
                else
                {
                    if (auxiliar_xi[0] < auxiliar_xi[1])
                    {
                        auxiliar_coordinates[0] = auxiliar_xi[0];
                        auxiliar_coordinates[1] = auxiliar_xi[1];
                    }
                    else
                    {
                        auxiliar_coordinates[1] = auxiliar_xi[0];
                        auxiliar_coordinates[0] = auxiliar_xi[1];
                    }
                }
            }
            else
            {
                if (mDebugGeometries == true)
                {
                    KRATOS_WATCH(OriginalSlaveGeometry);
                    KRATOS_WATCH(OriginalMasterGeometry);
                    KRATOS_ERROR << "WARNING: THIS IS NOT SUPPOSED TO HAPPEN!!!! (TYPE 1)" << std::endl;
                }
                return false; // NOTE: Giving problems
            }
            
            total_weight = auxiliar_coordinates[1] - auxiliar_coordinates[0];
        }
        
        if(total_weight < 0.0)
        {
            KRATOS_ERROR << "WAAAAAAAAAAAAARNING!!!!!!!!, wrong order of the coordinates: "<< auxiliar_coordinates << std::endl;
        }
        else if(total_weight > 2.0)
        {
            KRATOS_ERROR << "WAAAAAAAAAAAAARNING!!!!!!!!, impossible, Weight higher than 2: "<< auxiliar_coordinates << std::endl;
        }
        
        if (total_weight > std::numeric_limits<double>::epsilon())
        {
            ConditionsPointsSlave.resize(1);
            array_1d<PointType, 2> list_points;
            list_points[0].Coordinate(1) = auxiliar_coordinates[0];
            list_points[1].Coordinate(1) = auxiliar_coordinates[1];
            ConditionsPointsSlave[0] = list_points;
            
            return true;
        }
        else
        {
            ConditionsPointsSlave.clear();
            return false;
        }
    
        ConditionsPointsSlave.clear();
        return false;
    }
    
    /***********************************************************************************/
    /***********************************************************************************/

    template<>  
    inline bool ExactMortarIntegrationUtility<3,3>::GetExactIntegration(    
        GeometryNodeType& OriginalSlaveGeometry,
        const array_1d<double, 3>& SlaveNormal,
        GeometryNodeType& OriginalMasterGeometry,
        const array_1d<double, 3>& MasterNormal,
        ConditionArrayListType& ConditionsPointsSlave
        )
    {
        // Firt we create an auxiliar plane based in the condition center and its normal
        const PointType slave_center = OriginalSlaveGeometry.Center();
        
        // We define the condition tangents
        const array_1d<double, 3> slave_tangent_xi  = (OriginalSlaveGeometry[1].Coordinates() - OriginalSlaveGeometry[0].Coordinates())/norm_2(OriginalSlaveGeometry[1].Coordinates() - OriginalSlaveGeometry[0].Coordinates());
        const array_1d<double, 3> slave_tangent_eta = MathUtils<double>::UnitCrossProduct(slave_tangent_xi, SlaveNormal);
        
        // We define the tolerance
        const double tolerance = 1.0e-8; // std::numeric_limits<double>::epsilon();
        
        // We define the auxiliar geometry
        std::vector<PointType::Pointer> points_array_slave  (3);
        std::vector<PointType::Pointer> points_array_master (3);
        for (unsigned int i_node = 0; i_node < 3; i_node++)
        {
            PointType aux_point;
            
            aux_point.Coordinates() = OriginalSlaveGeometry[i_node].Coordinates(); // NOTE: We are in a linear triangle, all the nodes belong already to the plane, so, the step one can be avoided, we directly project  the master nodes
            MortarUtilities::RotatePoint( aux_point, slave_center, slave_tangent_xi, slave_tangent_eta, false);
            points_array_slave[i_node] = boost::make_shared<PointType>(aux_point);
            
            aux_point = MortarUtilities::FastProject(slave_center, OriginalMasterGeometry[i_node], SlaveNormal);
            MortarUtilities::RotatePoint( aux_point, slave_center, slave_tangent_xi, slave_tangent_eta, false);
            points_array_master[i_node] = boost::make_shared<PointType>(aux_point);
        }
        
        Triangle3D3 <PointType> slave_geometry(  points_array_slave  );
        Triangle3D3 <PointType> master_geometry( points_array_master );
        
        // No we project both nodes from the slave side and the master side
        array_1d<bool, 3> all_inside;
        
        // We check if the nodes are inside
        CheckInside(all_inside, slave_geometry, master_geometry, tolerance);
        
        // We create the pointlist
        std::vector<PointType> point_list;
        
        // All the points inside
        if ((all_inside[0] == true) &&
            (all_inside[1] == true) &&
            (all_inside[2] == true))
        {
            ConditionsPointsSlave.resize(1);
            
            for (unsigned int i_node = 0; i_node < 3; i_node++)
            {
                PointType point;
                OriginalSlaveGeometry.PointLocalCoordinates(point, OriginalMasterGeometry[i_node]);
                ConditionsPointsSlave[0][i_node] = point;
            }
            
            return true;
        }
        else
        {       
            // We add the internal nodes
            PushBackPoints(point_list, all_inside, master_geometry);
            
            // We check if the nodes are inside
            CheckInside(all_inside, master_geometry, slave_geometry, tolerance);
            
            // We add the internal nodes
            PushBackPoints(point_list, all_inside, slave_geometry);
            
            return TriangleIntersections(ConditionsPointsSlave, point_list, OriginalSlaveGeometry, OriginalMasterGeometry, slave_geometry, master_geometry, slave_tangent_xi, slave_tangent_eta, slave_center);
        }
        
        ConditionsPointsSlave.clear();
        return false;
    }
    
    /***********************************************************************************/
    /***********************************************************************************/

    template<>  
    inline bool ExactMortarIntegrationUtility<3,4>::GetExactIntegration(   
        GeometryNodeType& OriginalSlaveGeometry,
        const array_1d<double, 3>& SlaveNormal,
        GeometryNodeType& OriginalMasterGeometry,
        const array_1d<double, 3>& MasterNormal,
        ConditionArrayListType& ConditionsPointsSlave
        )
    {        
        // We define the tolerance
        const double tolerance = 1.0e-8; // std::numeric_limits<double>::epsilon();
        
        // Firt we create an auxiliar plane based in the condition center and its normal
        const PointType slave_center = OriginalSlaveGeometry.Center();
        
        // We define the condition tangents
        const array_1d<double, 3> slave_tangent_xi  = (OriginalSlaveGeometry[2].Coordinates() - OriginalSlaveGeometry[0].Coordinates())/norm_2(OriginalSlaveGeometry[2].Coordinates() - OriginalSlaveGeometry[0].Coordinates());
        const array_1d<double, 3> slave_tangent_eta = MathUtils<double>::UnitCrossProduct(slave_tangent_xi, SlaveNormal);
        
        // We define the auxiliar geometry
        std::vector<PointType::Pointer> points_array_slave  (4);
        std::vector<PointType::Pointer> points_array_master (4);
        for (unsigned int i_node = 0; i_node < 4; i_node++)
        {
            PointType aux_point;
            
            aux_point = MortarUtilities::FastProject( slave_center,  OriginalSlaveGeometry[i_node], SlaveNormal);
            MortarUtilities::RotatePoint( aux_point, slave_center, slave_tangent_xi, slave_tangent_eta, false);
            points_array_slave[i_node] = boost::make_shared<PointType>(aux_point);
            
            aux_point = MortarUtilities::FastProject( slave_center,  OriginalMasterGeometry[i_node], SlaveNormal);
            MortarUtilities::RotatePoint( aux_point, slave_center, slave_tangent_xi, slave_tangent_eta, false);
            points_array_master[i_node] = boost::make_shared<PointType>(aux_point);
        }
        
        Quadrilateral3D4 <PointType> slave_geometry(  points_array_slave  );
        Quadrilateral3D4 <PointType> master_geometry( points_array_master );
        
        // No we project both nodes from the slave side and the master side
        array_1d<bool, 4> all_inside;
        
        // We check if the nodes are inside
        CheckInside(all_inside, slave_geometry, master_geometry, tolerance);
        
        // We create the pointlist
        std::vector<PointType> point_list;
        
        // All the points inside
        if ((all_inside[0] == true) &&
                 (all_inside[1] == true) &&
                 (all_inside[2] == true) &&
                 (all_inside[3] == true))
        {
            // We add the internal nodes
            PushBackPoints(point_list, all_inside, master_geometry);
            
            return TriangleIntersections(ConditionsPointsSlave, point_list, OriginalSlaveGeometry, OriginalMasterGeometry, slave_geometry, master_geometry, slave_tangent_xi, slave_tangent_eta, slave_center, true);
        }
        else
        {
            // We add the internal nodes
            PushBackPoints(point_list, all_inside, master_geometry);
            
            // We check if the nodes are inside
            CheckInside(all_inside, master_geometry, slave_geometry, tolerance);
            
            // We add the internal nodes
            PushBackPoints(point_list, all_inside, slave_geometry);
            
            return TriangleIntersections(ConditionsPointsSlave, point_list, OriginalSlaveGeometry, OriginalMasterGeometry, slave_geometry, master_geometry, slave_tangent_xi, slave_tangent_eta, slave_center);
        }
        
        ConditionsPointsSlave.clear();
        return false;
    }
}

#endif  /* KRATOS_EXACT_MORTAR_INTEGRATION_UTILITY_H_INCLUDED defined */
