use std::path::PathBuf;

use riddle::parse_problem;

#[test]
fn test_core_00() {
    let mut path = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    path.push("examples/core/example_00.rddl");

    let riddle = std::fs::read_to_string(path).expect("Failed to read RiDDLe file");
    let _ = parse_problem(&riddle).expect("Failed to parse problem");
}

#[test]
fn test_core_01() {
    let mut path = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    path.push("examples/core/example_01.rddl");

    let riddle = std::fs::read_to_string(path).expect("Failed to read RiDDLe file");
    let _ = parse_problem(&riddle).expect("Failed to parse problem");
}

#[test]
fn test_core_02() {
    let mut path = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    path.push("examples/core/example_02.rddl");

    let riddle = std::fs::read_to_string(path).expect("Failed to read RiDDLe file");
    let _ = parse_problem(&riddle).expect("Failed to parse problem");
}
