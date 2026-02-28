use std::{collections::VecDeque, iter::Peekable};

use crate::{
    language::{Class, Expr, Method, Predicate, Problem, Statement},
    lexer::{Lexer, Token},
};

pub mod language;
mod lexer;

pub struct Parser<'a> {
    lexer: Peekable<Lexer<'a>>,
    lookahead: VecDeque<Token>,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self {
        Parser { lexer: lexer.peekable(), lookahead: VecDeque::new() }
    }

    fn peek(&mut self) -> Option<&Token> {
        if let Some(token) = self.lookahead.back() { Some(token) } else { self.lexer.peek() }
    }

    fn peek_n(&mut self, n: usize) -> Option<&Token> {
        while self.lookahead.len() < n {
            if let Some(token) = self.lexer.next() {
                self.lookahead.push_back(token);
            } else {
                break;
            }
        }
        self.lookahead.get(n - 1)
    }

    fn next(&mut self) -> Option<Token> {
        if let Some(token) = self.lookahead.pop_front() { Some(token) } else { self.lexer.next() }
    }

    fn expect(&mut self, expected: Token) -> Result<Token, String> {
        match self.next() {
            Some(token) if token == expected => Ok(token),
            Some(token) => Err(format!("Expected {:?}, found {:?}", expected, token)),
            None => Err(format!("Expected {:?}, found end of input", expected)),
        }
    }

    pub fn parse_problem(&mut self) -> Result<Problem, String> {
        unimplemented!()
    }

    pub fn parse_class(&mut self) -> Result<Class, String> {
        unimplemented!()
    }

    pub fn parse_method(&mut self) -> Result<Method, String> {
        unimplemented!()
    }

    pub fn parse_predicate(&mut self) -> Result<Predicate, String> {
        unimplemented!()
    }

    pub fn parse_statement(&mut self) -> Result<Statement, String> {
        unimplemented!()
    }

    pub fn parse_expression(&mut self) -> Result<Expr, String> {
        unimplemented!()
    }

    fn parse_multiplicative_expression(&mut self) -> Result<Expr, String> {
        let mut factors = vec![self.parse_primary_expression()?];
        while let Some(token) = self.peek() {
            match token {
                Token::Asterisk => {
                    self.expect(Token::Asterisk)?; // consume '*'
                    factors.push(self.parse_primary_expression()?);
                }
                Token::Slash => {
                    self.expect(Token::Slash)?; // consume '/'
                    let right = self.parse_primary_expression()?;
                    let left = factors.pop().unwrap();
                    return Ok(Expr::Div { left: Box::new(left), right: Box::new(right) });
                }
                _ => break,
            }
        }
        if factors.len() == 1 { Ok(factors.remove(0)) } else { Ok(Expr::Mul { factors }) }
    }

    fn parse_primary_expression(&mut self) -> Result<Expr, String> {
        match self.next() {
            Some(Token::BoolLiteral(value)) => Ok(Expr::Bool(value)),
            Some(Token::IntLiteral(value)) => Ok(Expr::Int(value)),
            Some(Token::RealLiteral(int_part, frac_part)) => Ok(Expr::Real(int_part, frac_part)),
            Some(Token::Identifier(name)) => {
                let mut ids = vec![name];
                while let Some(Token::Dot) = self.peek() {
                    self.expect(Token::Dot)?; // consume '.'
                    if let Some(Token::Identifier(next_name)) = self.next() {
                        ids.push(next_name);
                    } else {
                        return Err("Expected identifier after '.'".to_string());
                    }
                }
                if let Some(Token::LParen) = self.peek() {
                    self.expect(Token::LParen)?;
                    let mut exprs = Vec::new();
                    while !matches!(self.peek(), Some(Token::RParen)) {
                        exprs.push(self.parse_expression()?);
                        if let Some(Token::Comma) = self.peek() {
                            self.expect(Token::Comma)?; // consume ','
                        } else {
                            break;
                        }
                    }
                    self.expect(Token::RParen)?;
                    Ok(Expr::Function { name: ids, args: exprs })
                } else {
                    Ok(Expr::QualifiedId { ids })
                }
            }
            Some(Token::LParen) => {
                let expr = self.parse_expression()?;
                self.expect(Token::RParen)?;
                Ok(expr)
            }
            Some(token) => Err(format!("Unexpected token: {:?}", token)),
            None => Err("Unexpected end of input".to_string()),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn parse_primary_expression(input: &str) -> Expr {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        parser.parse_primary_expression().expect("Failed to parse primary expression")
    }

    #[test]
    fn test_primary_expressions() {
        assert_eq!(parse_primary_expression("true"), Expr::Bool(true));
        assert_eq!(parse_primary_expression("false"), Expr::Bool(false));
        assert_eq!(parse_primary_expression("123"), Expr::Int(123));
        assert_eq!(parse_primary_expression("12.34"), Expr::Real(1234, 100));
        assert_eq!(parse_primary_expression("foo"), Expr::QualifiedId { ids: vec!["foo".to_string()] });
        assert_eq!(parse_primary_expression("foo.bar"), Expr::QualifiedId { ids: vec!["foo".to_string(), "bar".to_string()] });
        // assert_eq!(parse_primary_expression("(123)"), Expr::Int(123));
        assert_eq!(parse_primary_expression("f()"), Expr::Function { name: vec!["f".to_string()], args: vec![] });
        // assert_eq!(parse_primary_expression("g(1, true)"), Expr::Function { name: vec!["g".to_string()], args: vec![Expr::Int(1), Expr::Bool(true)] });
        // assert_eq!(parse_primary_expression("Math.max(1, 2)"), Expr::Function { name: vec!["Math".to_string(), "max".to_string()], args: vec![Expr::Int(1), Expr::Int(2)] });
    }
}
