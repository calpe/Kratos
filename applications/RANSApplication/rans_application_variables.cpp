//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Suneth Warnakulasuriya (https://github.com/sunethwarna)
//

#include "rans_application_variables.h"

namespace Kratos
{
    // incompressible potential flow specific variables
    KRATOS_CREATE_VARIABLE( double, VELOCITY_POTENTIAL )
    KRATOS_CREATE_VARIABLE( double, PRESSURE_POTENTIAL )
    KRATOS_CREATE_VARIABLE( int, RANS_IS_INLET )
    KRATOS_CREATE_VARIABLE( int, RANS_IS_OUTLET )
    KRATOS_CREATE_VARIABLE( int, RANS_IS_STRUCTURE )
}
