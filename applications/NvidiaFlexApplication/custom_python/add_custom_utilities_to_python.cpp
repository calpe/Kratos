//
// Author: Salva Latorre latorre@cimne.upc.edu
//

// External includes
#include <pybind11/pybind11.h>

// Project includes
#include "custom_python/add_custom_utilities_to_python.h"
#include "custom_utilities/flex_wrapper.h"

namespace Kratos {

    namespace Python {

        using namespace pybind11;

        void AddCustomUtilitiesToPython(pybind11::module& m) {

        class_<FlexWrapper, FlexWrapper::Pointer>(m, "FlexWrapper")
            .def(init<ModelPart&, ParticleCreatorDestructor&>())
            .def("UpdateFlex", &FlexWrapper::UpdateFlex)
            .def("TransferDataFromFlexToKratos", &FlexWrapper::TransferDataFromFlexToKratos)
            .def("SolveTimeSteps", &FlexWrapper::SolveTimeSteps)
            ;
        }
    }  // namespace Python
} // Namespace Kratos
