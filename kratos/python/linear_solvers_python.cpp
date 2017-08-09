//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Pooyan Dadvand
//                   Riccardo Rossi
//


// System includes


// External includes
#include <boost/python.hpp>


// Project includes
#include "includes/define.h"
#include "includes/tracer.h"
#include "linear_solvers_python.h"
#include "linear_solvers/kratos_linear_solvers.h"
#include "vectorial_spaces/kratos_vectorial_spaces.h"


namespace Kratos
{

namespace Python
{

typedef Kratos::LinearSolver<Kratos::CSRSpace<double>, Kratos::DenseSpace<double> > KratosLinearSolverType;

typedef Kratos::DirectSolver<Kratos::CSRSpace<double>, Kratos::DenseSpace<double> > KratosDirectSolverType;

typedef Kratos::IterativeSolver<Kratos::CSRSpace<double>, Kratos::DenseSpace<double> > KratosIterativeSolverType;

#define KRATOS_REGISTER_LINEAR_SOLVERS_BEGIN using namespace Kratos;

#define KRATOS_REGISTER_LINEAR_SOLVER typedef
#define KRATOS_AS_LINEAR_SOLVER_NAMED(name) KRATOS_TYPE_NAME_OF(name); \
    bool (name##Type::*name##_solve_method_pointer)(name##Type::SparseMatrixType&, name##Type::VectorType&, name##Type::VectorType&) = &name##Type::Solve; \
       class_<name##Type, bases<name##Type::BaseType> >(#name) \
	 .def("Solve", name##_solve_method_pointer) \
	 .def(self_ns::str(self)) \
 
#define KRATOS_REGISTER_PRECONDITIONER typedef
#define KRATOS_AS_PRECONDITIONER_NAMED(name) KRATOS_TYPE_NAME_OF(name); \
    void (name##Type::*name##_initialize_method_pointer)(name##Type::SparseMatrixType&, name##Type::VectorType&, name##Type::VectorType&) = &name##Type::Initialize; \
       class_<name##Type, bases<name##Type::BaseType> >(#name) \
	 .def("Initialize", name##_initialize_method_pointer) \
	 .def(self_ns::str(self)) \
 
const KratosIterativeSolverType::PreconditionerType::Pointer  (KratosIterativeSolverType::*IterativeSolver_get_preconditioner_method_pointer)(void) const = &KratosIterativeSolverType::GetPreconditioner;

#define KRATOS_AS_ITERATIVE_LINEAR_SOLVER_NAMED(name) \
KRATOS_AS_LINEAR_SOLVER_NAMED(name) \
KRATOS_LINEAR_SOLVER_ADDITIONAL_METHOD(.def(init<double>())) \
KRATOS_LINEAR_SOLVER_ADDITIONAL_METHOD(.def(init<double,unsigned int>())) \
KRATOS_LINEAR_SOLVER_ADDITIONAL_METHOD(.def(init<double,unsigned int, Preconditioner<CSRSpace<double>, DenseSpace<double> >::Pointer >()))

#define KRATOS_LINEAR_SOLVER_ADDITIONAL_METHOD(method) method

//   void Reorder(CSRMatrix<double>& rA, Vector<double>& rX, Vector<double>& rB)
using namespace boost::python;

void  AddLinearSolversToPython()
{
    class_<Kratos::Preconditioner<Kratos::CSRSpace<double>, Kratos::DenseSpace<double> > >("Preconditioner", no_init)
    .def(self_ns::str(self))
    ;

    class_<Kratos::CuthillMcKeeReorderer<Kratos::CSRSpace<double>, Kratos::DenseSpace<double> > >("CuthillMcKeeReorderer")
    .def("Initialize" , &Kratos::CuthillMcKeeReorderer<Kratos::CSRSpace<double>, Kratos::DenseSpace<double> >::Initialize)
    .def("Reorder" , &Kratos::CuthillMcKeeReorderer<Kratos::CSRSpace<double>, Kratos::DenseSpace<double> >::Reorder)
    .def("InverseReorder" , &Kratos::CuthillMcKeeReorderer<Kratos::CSRSpace<double>, Kratos::DenseSpace<double> >::InverseReorder)
    ;

    class_<KratosLinearSolverType>("LinearSolver", no_init)
    .def(self_ns::str(self))
    ;

    class_<KratosDirectSolverType, bases<KratosLinearSolverType> >("DirectSolver",no_init)
    .def(self_ns::str(self))
    ;

    class_<KratosIterativeSolverType, bases<KratosLinearSolverType> >("IterativeSolver",no_init)
    .add_property("Preconditioner", IterativeSolver_get_preconditioner_method_pointer, &KratosIterativeSolverType::SetPreconditioner)
    .add_property("MaxIterationsNumber", &KratosIterativeSolverType::GetMaxIterationsNumber, &KratosIterativeSolverType::SetMaxIterationsNumber)
    .add_property("IterationsNumber", &KratosIterativeSolverType::GetIterationsNumber, &KratosIterativeSolverType::SetIterationsNumber)
    .add_property("Tolerance", &KratosIterativeSolverType::GetTolerance, &KratosIterativeSolverType::SetTolerance)
    .add_property("ResidualNorm", &KratosIterativeSolverType::GetResidualNorm, &KratosIterativeSolverType::SetResidualNorm)
    .def("IterationNeeded", &KratosIterativeSolverType::IterationNeeded)
    .def("IsConverged", &KratosIterativeSolverType::IsConverged)
    .def(self_ns::str(self))
    ;

#include "components/linear_solvers.h"

}


}  // namespace Python.

} // Namespace Kratos

