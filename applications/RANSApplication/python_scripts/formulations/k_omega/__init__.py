from __future__ import print_function, absolute_import, division

# import kratos
import KratosMultiphysics as Kratos
from KratosMultiphysics.process_factory import KratosProcessFactory

# import RANS
import KratosMultiphysics.RANSApplication as KratosRANS
from KratosMultiphysics.RANSApplication import RansVariableUtilities

# import formulation interface
from KratosMultiphysics.RANSApplication.formulations.formulation import Formulation

# import formulations
from .k_omega_k_formulation import KOmegaKFormulation
from .k_omega_omega_formulation import KOmegaOmegaFormulation

# import utilities
from KratosMultiphysics.RANSApplication import RansCalculationUtilities
from KratosMultiphysics.RANSApplication import ScalarVariableDifferenceNormCalculationUtility
from KratosMultiphysics.RANSApplication.formulations.utilities import GetConvergenceInfo

class KOmegaFormulation(Formulation):
    def __init__(self, model_part, settings):
        super(KOmegaFormulation, self).__init__(model_part, settings)

        default_settings = Kratos.Parameters(r'''
        {
            "formulation_name": "k_omega",
            "stabilization_method": "algebraic_flux_corrected",
            "turbulent_kinetic_energy_solver_settings": {},
            "turbulent_specific_energy_dissipation_rate_solver_settings": {},
            "wall_function_properties":{
                "y_plus_calculation_method": "calculated",
                "y_plus_value": 11.06
            },
            "coupling_settings":
            {
                "relative_tolerance": 1e-3,
                "absolute_tolerance": 1e-5,
                "max_iterations": 10
            },
            "auxiliar_process_list": [],
            "echo_level": 0
        }''')
        self.settings.ValidateAndAssignDefaults(default_settings)

        self.stabilization_method = self.settings["stabilization_method"].GetString()

        self.tke_formulation = KOmegaKFormulation(model_part, settings["turbulent_kinetic_energy_solver_settings"])
        self.tke_formulation.SetStabilizationMethod(self.stabilization_method)
        self.AddFormulation(self.tke_formulation)

        self.omega_formulation = KOmegaOmegaFormulation(model_part, settings["turbulent_specific_energy_dissipation_rate_solver_settings"])
        self.omega_formulation.SetStabilizationMethod(self.stabilization_method)
        self.AddFormulation(self.omega_formulation)

        self.echo_level = self.settings["echo_level"].GetInt()
        self.nu_t_convergence_utility = ScalarVariableDifferenceNormCalculationUtility(self.GetBaseModelPart(), Kratos.TURBULENT_VISCOSITY)
        self.SetMaxCouplingIterations(self.settings["coupling_settings"]["max_iterations"].GetInt())

    def AddVariables(self):
        self.GetBaseModelPart().AddNodalSolutionStepVariable(Kratos.DENSITY)
        self.GetBaseModelPart().AddNodalSolutionStepVariable(Kratos.VELOCITY)
        self.GetBaseModelPart().AddNodalSolutionStepVariable(Kratos.MESH_VELOCITY)
        self.GetBaseModelPart().AddNodalSolutionStepVariable(Kratos.NORMAL)
        self.GetBaseModelPart().AddNodalSolutionStepVariable(Kratos.VISCOSITY)
        self.GetBaseModelPart().AddNodalSolutionStepVariable(Kratos.KINEMATIC_VISCOSITY)
        self.GetBaseModelPart().AddNodalSolutionStepVariable(Kratos.TURBULENT_VISCOSITY)
        self.GetBaseModelPart().AddNodalSolutionStepVariable(KratosRANS.RANS_Y_PLUS)
        self.GetBaseModelPart().AddNodalSolutionStepVariable(KratosRANS.TURBULENT_KINETIC_ENERGY)
        self.GetBaseModelPart().AddNodalSolutionStepVariable(KratosRANS.TURBULENT_KINETIC_ENERGY_RATE)
        self.GetBaseModelPart().AddNodalSolutionStepVariable(KratosRANS.TURBULENT_SPECIFIC_ENERGY_DISSIPATION_RATE)
        self.GetBaseModelPart().AddNodalSolutionStepVariable(KratosRANS.TURBULENT_SPECIFIC_ENERGY_DISSIPATION_RATE_2)
        self.GetBaseModelPart().AddNodalSolutionStepVariable(KratosRANS.RANS_AUXILIARY_VARIABLE_1)
        self.GetBaseModelPart().AddNodalSolutionStepVariable(KratosRANS.RANS_AUXILIARY_VARIABLE_2)

        if (self.stabilization_method == "algebraic_flux_corrected"):
            self.GetBaseModelPart().AddNodalSolutionStepVariable(KratosRANS.AFC_POSITIVE_ANTI_DIFFUSIVE_FLUX)
            self.GetBaseModelPart().AddNodalSolutionStepVariable(KratosRANS.AFC_NEGATIVE_ANTI_DIFFUSIVE_FLUX)
            self.GetBaseModelPart().AddNodalSolutionStepVariable(KratosRANS.AFC_POSITIVE_ANTI_DIFFUSIVE_FLUX_LIMIT)
            self.GetBaseModelPart().AddNodalSolutionStepVariable(KratosRANS.AFC_NEGATIVE_ANTI_DIFFUSIVE_FLUX_LIMIT)

        Kratos.Logger.PrintInfo(self.GetName(), "Added solution step variables.")

    def AddDofs(self):
        Kratos.VariableUtils().AddDof(KratosRANS.TURBULENT_KINETIC_ENERGY, self.GetBaseModelPart())
        Kratos.VariableUtils().AddDof(KratosRANS.TURBULENT_SPECIFIC_ENERGY_DISSIPATION_RATE, self.GetBaseModelPart())

        Kratos.Logger.PrintInfo(self.GetName(), "Added solution step dofs.")

    def GetMinimumBufferSize(self):
        if (self.is_steady_simulation):
            return 1
        else:
            return 2

    def Initialize(self):
        model_part = self.GetBaseModelPart()
        model = model_part.GetModel()

        process_info = model_part.ProcessInfo
        wall_model_part_name = process_info[KratosRANS.WALL_MODEL_PART_NAME]
        kappa = process_info[KratosRANS.WALL_VON_KARMAN]

        nut_process = KratosRANS.RansNutKOmegaUpdateProcess(
                                            model,
                                            self.GetBaseModelPart().Name,
                                            1e-12,
                                            self.echo_level)
        self.AddProcess(nut_process)

        nut_wall_process = KratosRANS.RansNutYPlusWallFunctionUpdateProcess(
                                            model,
                                            wall_model_part_name,
                                            kappa,
                                            1e-12,
                                            self.echo_level)
        self.AddProcess(nut_wall_process)

        factory = KratosProcessFactory(self.GetBaseModelPart().GetModel())
        self.auxiliar_process_list = factory.ConstructListOfProcesses(
            self.settings["auxiliar_process_list"])
        for process in self.auxiliar_process_list:
            self.AddProcess(process)

        super(KOmegaFormulation, self).Initialize()

    def SetConstants(self, settings):
        defaults = Kratos.Parameters('''{
            "wall_smoothness_beta"                          : 5.2,
            "von_karman"                                    : 0.41,
            "c_mu"                                          : 0.09,
            "c1"                                            : 0.1,
            "beta_zero"                                     : 0.0708,
            "gamma"                                         : 0.52,
            "sigma_k"                                       : 0.6,
            "sigma_omega"                                   : 0.5,
            "y_plus_lower_limit"                            : 2.0,
            "stabilizing_upwind_operator_coefficient"       : 1.2,
            "stabilizing_positivity_preserving_coefficient" : 1.2
        }''')

        settings.ValidateAndAssignDefaults(defaults)

        process_info = self.GetBaseModelPart().ProcessInfo
        process_info.SetValue(KratosRANS.WALL_SMOOTHNESS_BETA, settings["wall_smoothness_beta"].GetDouble())
        process_info.SetValue(KratosRANS.WALL_VON_KARMAN, settings["von_karman"].GetDouble())
        process_info.SetValue(KratosRANS.TURBULENCE_RANS_C_MU, settings["c_mu"].GetDouble())
        process_info.SetValue(KratosRANS.TURBULENCE_RANS_K_C1, settings["c1"].GetDouble())
        process_info.SetValue(KratosRANS.TURBULENCE_RANS_BETA_ZERO, settings["beta_zero"].GetDouble())
        process_info.SetValue(KratosRANS.TURBULENCE_RANS_GAMMA, settings["gamma"].GetDouble())
        process_info.SetValue(KratosRANS.TURBULENT_KINETIC_ENERGY_SIGMA, settings["sigma_k"].GetDouble())
        process_info.SetValue(KratosRANS.TURBULENT_SPECIFIC_ENERGY_DISSIPATION_RATE_SIGMA, settings["sigma_omega"].GetDouble())
        process_info.SetValue(KratosRANS.RANS_Y_PLUS_LOWER_LIMIT, settings["y_plus_lower_limit"].GetDouble())
        process_info.SetValue(KratosRANS.RANS_STABILIZATION_DISCRETE_UPWIND_OPERATOR_COEFFICIENT, settings["stabilizing_upwind_operator_coefficient"].GetDouble())
        process_info.SetValue(KratosRANS.RANS_STABILIZATION_DIAGONAL_POSITIVITY_PRESERVING_COEFFICIENT, settings["stabilizing_positivity_preserving_coefficient"].GetDouble())
        process_info.SetValue(KratosRANS.RANS_Y_PLUS_LIMIT, RansCalculationUtilities.CalculateLogarithmicYPlusLimit(
                                                                                    process_info[KratosRANS.WALL_VON_KARMAN],
                                                                                    process_info[KratosRANS.WALL_SMOOTHNESS_BETA]
                                                                                    ))

    def SetTimeSchemeSettings(self, settings):
        if (settings.Has("scheme_type")):
            scheme_type = settings["scheme_type"].GetString()
            if (scheme_type == "steady"):
                self.is_steady_simulation = True
                self.GetBaseModelPart().ProcessInfo.SetValue(Kratos.BOSSAK_ALPHA, 0.0)
            elif (scheme_type == "transient"):
                self.is_steady_simulation = False
                default_settings = Kratos.Parameters('''{
                    "scheme_type": "transient",
                    "alpha_bossak": -0.3
                }''')
                settings.ValidateAndAssignDefaults(default_settings)
                self.GetBaseModelPart().ProcessInfo.SetValue(Kratos.BOSSAK_ALPHA, settings["alpha_bossak"].GetDouble())
            else:
                raise Exception("Only \"steady\" and \"transient\" scheme types supported. [ scheme_type = \"" + scheme_type  + "\" ]")
        else:
            raise Exception("\"scheme_type\" is missing in time scheme settings")

        super(KOmegaFormulation, self).SetTimeSchemeSettings(settings)

    def SolveCouplingStep(self):
        relative_tolerance = self.settings["coupling_settings"]["relative_tolerance"].GetDouble()
        absolute_tolerance = self.settings["coupling_settings"]["absolute_tolerance"].GetDouble()
        max_iterations = self.GetMaxCouplingIterations()

        for iteration in range(max_iterations):
            self.nu_t_convergence_utility.InitializeCalculation()

            for formulation in self.list_of_formulations:
                if (not formulation.SolveCouplingStep()):
                    return False
            self.ExecuteAfterCouplingSolveStep()

            relative_error, absolute_error = self.nu_t_convergence_utility.CalculateDifferenceNorm()
            info = GetConvergenceInfo(Kratos.TURBULENT_VISCOSITY, relative_error, relative_tolerance, absolute_error, absolute_tolerance)
            Kratos.Logger.PrintInfo(self.GetName() + " CONVERGENCE", info)

            is_converged = relative_error < relative_tolerance or absolute_error < absolute_tolerance
            if (is_converged):
                Kratos.Logger.PrintInfo(self.GetName() + " CONVERGENCE", "TURBULENT_VISCOSITY *** CONVERGENCE ACHIEVED ***")

            Kratos.Logger.PrintInfo(self.GetName(), "Solved coupling itr. " + str(iteration + 1) + "/" + str(max_iterations) + ".")
            if (is_converged):
                return True

        return True