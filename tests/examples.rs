use riddle::parse_problem;
use std::fs::read_to_string;
use std::path::PathBuf;

macro_rules! test_riddle {
    ($name:ident, $path:expr) => {
        #[test]
        fn $name() {
            let mut path = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
            path.push($path);
            let content = read_to_string(path).expect(&format!("Failed to read file: {}", $path));
            parse_problem(&content).expect("Failed to parse problem");
        }
    };
}

test_riddle!(test_core_00, "examples/core/example_00.rddl");
test_riddle!(test_core_01, "examples/core/example_01.rddl");
test_riddle!(test_core_02, "examples/core/example_02.rddl");
test_riddle!(test_core_03, "examples/core/example_03.rddl");
test_riddle!(test_core_04, "examples/core/example_04.rddl");
test_riddle!(test_core_05, "examples/core/example_05.rddl");
test_riddle!(test_core_06, "examples/core/example_06.rddl");
test_riddle!(test_core_07, "examples/core/example_07.rddl");
test_riddle!(test_core_08, "examples/core/example_08.rddl");
test_riddle!(test_core_09, "examples/core/example_09.rddl");
test_riddle!(test_core_10, "examples/core/example_10.rddl");
test_riddle!(test_core_11, "examples/core/example_11.rddl");
test_riddle!(test_core_12, "examples/core/example_12.rddl");
test_riddle!(test_core_13, "examples/core/example_13.rddl");

test_riddle!(blocks_domain, "examples/blocks/blocks_domain.rddl");
test_riddle!(blocks_01, "examples/blocks/blocks_01.rddl");
test_riddle!(blocks_02, "examples/blocks/blocks_02.rddl");
test_riddle!(blocks_03, "examples/blocks/blocks_03.rddl");

test_riddle!(types_rr_rr0, "examples/types/rr/rr_0.rddl");
test_riddle!(types_rr_rr1, "examples/types/rr/rr_1.rddl");
test_riddle!(types_rr_rr2, "examples/types/rr/rr_2.rddl");
test_riddle!(types_rr_rr3, "examples/types/rr/rr_3.rddl");
test_riddle!(types_sv_sv0, "examples/types/sv/sv_0.rddl");
test_riddle!(types_sv_sv1, "examples/types/sv/sv_1.rddl");
test_riddle!(types_sv_sv2, "examples/types/sv/sv_2.rddl");
test_riddle!(types_sv_sv3, "examples/types/sv/sv_3.rddl");