use crate::{
    language::{ClassDef, ConstructorDef, Expr, MethodDef, PredicateDef, ProblemDef, RiddleError, Statement},
    lexer::Lexer,
    parser::Parser,
};
pub use serde_json;
use serde_json::Value;

pub mod core;
pub mod env;
pub mod language;
mod lexer;
mod parser;
pub mod scope;

pub fn parse_problem(input: &str) -> Result<ProblemDef, RiddleError> {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_problem()
}

pub fn parse_class(input: &str) -> Result<ClassDef, RiddleError> {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_class()
}

pub fn parse_constructor(input: &str) -> Result<ConstructorDef, RiddleError> {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_constructor()
}

pub fn parse_method(input: &str) -> Result<MethodDef, RiddleError> {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_method()
}

pub fn parse_predicate(input: &str) -> Result<PredicateDef, RiddleError> {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_predicate()
}

pub fn parse_statement(input: &str) -> Result<Statement, RiddleError> {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_statement()
}

pub fn parse_expression(input: &str) -> Result<Expr, RiddleError> {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_expression()
}

pub trait ToJson {
    fn to_json(&self) -> Value;
}
