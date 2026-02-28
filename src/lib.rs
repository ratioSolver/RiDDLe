use crate::{
    language::{Expr, PredicateDef, Statement},
    lexer::Lexer,
    parser::Parser,
};

pub mod language;
mod lexer;
mod parser;

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
