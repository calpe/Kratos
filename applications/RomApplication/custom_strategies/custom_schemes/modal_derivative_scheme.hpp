//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                     Kratos default license: kratos/license.txt
//
//  Main authors:    Altug Emiroglu, https://github.com/emiroglu
//


#if !defined(KRATOS_ROM_MODAL_DERIVATIVE_SCHEME )
#define  KRATOS_ROM_MODAL_DERIVATIVE_SCHEME

/* System includes */

/* External includes */

/* Project includes */
#include "includes/define.h"
#include "includes/model_part.h"
#include "includes/ublas_interface.h"
#include "utilities/entities_utilities.h"
#include "solving_strategies/schemes/scheme.h"

/* Application includes */
#include "rom_application_variables.h"

namespace Kratos
{
///@name Kratos Globals
///@{
///@}
///@name Type Definitions
///@{
///@}
///@name  Enum's
///@{
///@}
///@name  Functions
///@{
///@}
///@name Kratos Classes
///@{

/**
 * @class ModalDerivativeScheme
 * @ingroup KratosCore
 * @brief This class provides the implementation of the basic tasks that are needed by the solution strategy.
 * @details It is intended to be the place for tailoring the solution strategies to problem specific tasks.
 * @tparam TSparseSpace The sparse space considered
 * @tparam TDenseSpace The dense space considered
 * @author Altug Emiroglu
 */
template<class TSparseSpace,
         class TDenseSpace //= DenseSpace<double>
         >
class ModalDerivativeScheme : public Scheme<TSparseSpace,TDenseSpace>
{
public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition of ModalDerivativeScheme
    KRATOS_CLASS_POINTER_DEFINITION(ModalDerivativeScheme);

    // Base type definition
    typedef Scheme<TSparseSpace,TDenseSpace> BaseType;

    /// Matrix type definition
    typedef typename BaseType::TSystemMatrixType TSystemMatrixType;
    /// Vector type definition
    typedef typename BaseType::TSystemVectorType TSystemVectorType;
    /// Local system matrix type definition
    typedef typename BaseType::LocalSystemMatrixType LocalSystemMatrixType;
    /// Local system vector type definition
    typedef typename BaseType::LocalSystemVectorType LocalSystemVectorType;

    /// DoF type definition
    typedef Dof<double> TDofType;
    /// Dof pointers vector type
    typedef std::vector<TDofType::Pointer> TElementDofPointersVectorType;

    /// Node type definition
    typedef Node<3>::NodeType TNodeType;
    /// Node pointer type definition
    typedef TNodeType::Pointer TNodePointerType;
    /// Node pointers vector type
    typedef std::vector<TNodePointerType> TNodePointerVectorType;

    ///@}
    ///@name Life Cycle
    ///@{

        /**
     * @brief Default Constructor
     * @details Initiliazes the flags
     */
    /// Constructor.
    explicit ModalDerivativeScheme(Parameters InputParameters)
    : 
    Scheme<TSparseSpace,TDenseSpace>()
    {
        KRATOS_TRY

        std::string finite_difference_type = InputParameters["finite_difference_type"].GetString();
        if (finite_difference_type == "forward")
            mFiniteDifferenceType = FiniteDifferenceType::Forward;            
        else if (finite_difference_type == "central")
            mFiniteDifferenceType = FiniteDifferenceType::Central;
        else
            KRATOS_ERROR << "\"finite_difference_type\" can only be \"forward\" or \"central\""  << std::endl;

        mFiniteDifferenceStepSize = InputParameters["finite_difference_step_size"].GetDouble();

        KRATOS_CATCH("")
    }

    /// Destructor.
    ~ModalDerivativeScheme() override 
    {
    }

    ///@}
    ///@name Operators
    ///@{

    ///@}
    ///@name Operations
    ///@{

    /**
     * @brief Functions totally analogous to the precedent but applied to the "condition" objects
     * @param rCondition The condition to compute
     * @param RHS_Contribution The RHS vector contribution
     * @param rEquationIdVector The ID's of the condition degrees of freedom
     * @param rCurrentProcessInfo The current process info instance
     */
    void CalculateSystemContributions(
        Element& rElement,
        LocalSystemMatrixType& rLHS_Contribution,
        LocalSystemVectorType& rRHS_Contribution,
        Element::EquationIdVectorType& rEquationId,
        const ProcessInfo& rCurrentProcessInfo
        ) override
    {
        KRATOS_TRY

        this->CalculateLHSContribution(rElement, rLHS_Contribution, rEquationId, rCurrentProcessInfo);

        this->CalculateRHSContribution(rElement, rRHS_Contribution, rEquationId, rCurrentProcessInfo);

        rElement.EquationIdVector(rEquationId,rCurrentProcessInfo);

        KRATOS_CATCH("")
    }

    /**
     * @brief This function is designed to calculate just the LHS contribution
     * @param rElement The element to compute
     * @param LHS_Contribution The RHS vector contribution
     * @param rEquationIdVector The ID's of the element degrees of freedom
     * @param rCurrentProcessInfo The current process info instance
     */
    void CalculateLHSContribution(
        Element& rElement,
        LocalSystemMatrixType& rLHS_Contribution,
        Element::EquationIdVectorType& rEquationId,
        const ProcessInfo& rCurrentProcessInfo
        ) override
    {
        KRATOS_TRY

        if (rCurrentProcessInfo[BUILD_LEVEL] == 1 ) // Mass matrix
            rElement.CalculateMassMatrix(rLHS_Contribution, rCurrentProcessInfo);
        else if (rCurrentProcessInfo[BUILD_LEVEL] == 2) // Stiffness matrix
            rElement.CalculateLeftHandSide(rLHS_Contribution, rCurrentProcessInfo);
        else 
            KRATOS_ERROR <<"Invalid BUILD_LEVEL: " << rCurrentProcessInfo[BUILD_LEVEL] << std::endl;
        
        rElement.EquationIdVector(rEquationId, rCurrentProcessInfo);

        KRATOS_CATCH("")
    }

    /**
     * @brief This function is designed to calculate just the RHS contribution
     * @param rElement The element to compute
     * @param RHS_Contribution The RHS vector contribution
     * @param rEquationIdVector The ID's of the element degrees of freedom
     * @param rCurrentProcessInfo The current process info instance
     */
    void CalculateRHSContribution(
        Element& rElement,
        LocalSystemVectorType& rRHS_Contribution,
        Element::EquationIdVectorType& EquationId,
        const ProcessInfo& rCurrentProcessInfo
        ) override
    {
        KRATOS_TRY

        KRATOS_ERROR << "Calling CalculateRHSContribution from ModalDerivativeScheme base class!" << std::endl;

        KRATOS_CATCH("")
    }

    ///@}
    ///@name Access
    ///@{

    ///@}
    ///@name Inquiry
    ///@{

    ///@}
    ///@name Input and output
    ///@{

    ///@}
    ///@name Friends
    ///@{

    ///@}

protected:
    ///@name Protected static Member Variables
    ///@{

    ///@}
    ///@name Protected member Variables
    ///@{

    double mFiniteDifferenceStepSize;

    enum FiniteDifferenceType {Forward, Central};

    FiniteDifferenceType mFiniteDifferenceType;

    ///@}
    ///@name Protected Operators
    ///@{

    /**
     * @brief This function locks element nodes for finite differencing in parallel.
     * @details 
     * @param rElement The element to lock nodes
     */
    void LockElementNodes(Element& rElement)
    {
        KRATOS_TRY

        auto& element_nodes = rElement.GetGeometry();

        bool all_nodes_locked{false};
        auto it_node = element_nodes.begin();
        while (!all_nodes_locked)
        {   
            auto& node_lock = it_node->GetLock();
            // try to get a lock
            if (node_lock.try_lock())
            {
                // if successful: increment the node iterator
                ++it_node;

                // check if all the nodes are locked
                if (it_node == element_nodes.end())
                    all_nodes_locked = true;
            } 
            else
            {
                // if not successful: unlock all locked nodes and empty locked nodes vector
                for (auto it_locked_node = element_nodes.begin(); it_locked_node != it_node; ++it_locked_node)
                    it_locked_node->UnSetLock();
                    
                // reset the iterator to the first node
                it_node = element_nodes.begin();
            }
        }

        KRATOS_CATCH("")
    }

    /**
     * @brief This function unlocks element nodes after finite differencing in parallel.
     * @details 
     * @param rElement The element to unlock nodes
     */
    void UnlockElementNodes(Element& rElement)
    {
        KRATOS_TRY
        // Loop over element nodes
        for (auto& node_i : rElement.GetGeometry()) 
            node_i.UnSetLock();
        KRATOS_CATCH("")
    }

    /**
     * @brief This function retrieves the basis corresponding to element DOFs
     * @details 
     * @param rPhiElemental The element to unlock nodes
     * @param BasisIndex The index of the basis to retrieve
     * @param rElement The element to retrieve basis
     * @param rElementDofList The elemental dof list
     * @param rCurrentProcessInfo The current process info instance
     */
    void GetPhiElemental(
        LocalSystemVectorType& rPhiElemental,
        const std::size_t BasisIndex,
        const Element& rElement,       
        const ProcessInfo& rCurrentProcessInfo
    )
    {
        KRATOS_TRY

        // Get element DOF list
        TElementDofPointersVectorType r_element_dof_list;
        rElement.GetDofList(r_element_dof_list, rCurrentProcessInfo);

        // Get elemental and nodal DOFs size
        const std::size_t element_dofs_size = r_element_dof_list.size();
        const std::size_t nodal_dof_size = element_dofs_size/rElement.GetGeometry().size();
        
        // Get PhiElemental
        if (rPhiElemental.size() != element_dofs_size)
            rPhiElemental.resize(element_dofs_size, false);
        for(std::size_t i_node = 0; i_node < rElement.GetGeometry().size(); ++i_node)
        {
            const auto& r_node = rElement.GetGeometry()[i_node];
            const auto& r_phi_nodal = r_node.GetValue(ROM_BASIS);
            for (std::size_t i_dof = 0; i_dof < nodal_dof_size; ++i_dof)
            {
                const auto& rp_dof = r_element_dof_list[i_node*nodal_dof_size+i_dof];
                rPhiElemental[i_node*nodal_dof_size+i_dof] = r_phi_nodal(rCurrentProcessInfo[MAP_PHI].at(rp_dof->GetVariable().Key()), BasisIndex);
            }
        }
        KRATOS_CATCH("")
    }

    ///@}
    ///@name Protected Operations
    ///@{

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
    ///@name Private Inquiry
    ///@{

    ///@}
    ///@name Un accessible methods
    ///@{

    ///@}

}; // Class ModalDerivativeScheme

} // namespace Kratos.

#endif /* KRATOS_ROM_MODAL_DERIVATIVE_SCHEME  defined */


