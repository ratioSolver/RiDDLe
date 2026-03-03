use crate::{
    language::{Expr, PredicateDef, Statement},
    lexer::Lexer,
    parser::Parser,
};

pub mod env;
pub mod language;
mod lexer;
mod parser;

pub fn parse_problem(input: &str) -> Result<language::ProblemDef, String> {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_problem()
}

pub fn parse_enum(input: &str) -> Result<language::EnumDef, String> {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_enum()
}

pub fn parse_class(input: &str) -> Result<language::ClassDef, String> {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_class()
}

pub fn parse_constructor(input: &str) -> Result<language::ConstructorDef, String> {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_constructor()
}

pub fn parse_method(input: &str) -> Result<language::MethodDef, String> {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_method()
}

pub fn parse_predicate(input: &str) -> Result<PredicateDef, String> {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_predicate()
}

pub fn parse_statement(input: &str) -> Result<Statement, String> {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_statement()
}

pub fn parse_expression(input: &str) -> Expr {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_expression().expect("Failed to parse expression")
}
