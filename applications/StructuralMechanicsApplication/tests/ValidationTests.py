import structural_mechanics_test_factory

class SprismPanTests(structural_mechanics_test_factory.StructuralMechanichsTestFactory):
    file_name = "sprism_test/pan_test"

class PendulusTLTest(structural_mechanics_test_factory.StructuralMechanichsTestFactory):
    file_name = "pendulus_test/pendulus_TL_test"

class PendulusULTest(structural_mechanics_test_factory.StructuralMechanichsTestFactory):
    file_name = "pendulus_test/pendulus_UL_test"

class ShellT3AndQ4LinearStaticUnstructScordelisLoRoofTests(structural_mechanics_test_factory.StructuralMechanichsTestFactory):
    file_name = "shell_test/Shell_T3andQ4_linear_static_unstruct_scordelis_lo_roof"

class ShellT3AndQ4LinearStaticUnstructUnstructPinchedCylinderTests(structural_mechanics_test_factory.StructuralMechanichsTestFactory):
    file_name = "shell_test/Shell_T3andQ4_linear_static_unstruct_pinched_cylinder"

class ShellT3AndQ4LinearStaticUnstructPinchedHemisphereTests(structural_mechanics_test_factory.StructuralMechanichsTestFactory):
    file_name = "shell_test/Shell_T3andQ4_linear_static_unstruct_pinched_hemisphere"

class ShellT3AndQ4LinearStaticUnstructClampedCylinderOrthotropicTests(structural_mechanics_test_factory.StructuralMechanichsTestFactory):
    file_name = "shell_test/Shell_T3andQ4_linear_static_unstruct_clamped_cylinder_orthotropic"

class ShellT3AndQ4NonLinearStaticUnstructHingedCylRoofSnapthroughTests(structural_mechanics_test_factory.StructuralMechanichsTestFactory):
    file_name = "shell_test/Shell_T3andQ4_nonlinear_static_unstruct_hinged_cyl_roof_snapthrough"

class ShellT3AndQ4NonLinearStaticUnstructHingedCylRoofSnapthroughOrthotropicTests(structural_mechanics_test_factory.StructuralMechanichsTestFactory):
    file_name = "shell_test/Shell_T3andQ4_nonlinear_static_unstruct_hinged_cyl_roof_snapthrough_orthotropic"

class ShellT3AndQ4NonLinearDynamicUnstructOscillatingPlateTests(structural_mechanics_test_factory.StructuralMechanichsTestFactory):
    file_name = "shell_test/Shell_T3andQ4_nonlinear_dynamic_unstruct_oscillating_plate"

class ShellT3AndQ4NonLinearDynamicUnstructOscillatingPlateLumpedTests(structural_mechanics_test_factory.StructuralMechanichsTestFactory):
    file_name = "shell_test/Shell_T3andQ4_nonlinear_dynamic_unstruct_oscillating_plate_lumped"

class ShellT3AndQ4NonLinearDynamicStructPendulusTests(structural_mechanics_test_factory.StructuralMechanichsTestFactory):
    file_name = "shell_test/Shell_T3andQ4_nonlinear_dynamic_struct_pendulus"

class ShellT3AndQ4NonLinearDynamicStructPendulusLumpedTests(structural_mechanics_test_factory.StructuralMechanichsTestFactory):
    file_name = "shell_test/Shell_T3andQ4_nonlinear_dynamic_struct_pendulus_lumped"

class ShellT3AndQ4NonLinearDynamicUnstructPendulusTests(structural_mechanics_test_factory.StructuralMechanichsTestFactory):
    file_name = "shell_test/Shell_T3andQ4_nonlinear_dynamic_unstruct_pendulus"

class ShellT3AndQ4NonLinearDynamicUnstructPendulusLumpedTests(structural_mechanics_test_factory.StructuralMechanichsTestFactory):
    file_name = "shell_test/Shell_T3andQ4_nonlinear_dynamic_unstruct_pendulus_lumped"
