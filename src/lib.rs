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
    pub(crate) fn new(lexer: Lexer<'a>) -> Self {
        Parser { lexer: lexer.peekable(), lookahead: VecDeque::new() }
    }

    fn peek(&mut self, n: usize) -> Option<&Token> {
        while self.lookahead.len() <= n {
            if let Some(token) = self.lexer.next() {
                self.lookahead.push_back(token);
            } else {
                break;
            }
        }
        self.lookahead.get(n)
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

    fn parse_expression(&mut self) -> Result<Expr, String> {
        self.parse_or_expression()
    }

    fn parse_or_expression(&mut self) -> Result<Expr, String> {
        let mut terms = vec![self.parse_and_expression()?];
        while let Some(Token::Bar) = self.peek(0) {
            self.expect(Token::Bar)?; // consume '|'
            terms.push(self.parse_and_expression()?);
        }
        if terms.len() == 1 { Ok(terms.remove(0)) } else { Ok(Expr::Or { terms }) }
    }

    fn parse_and_expression(&mut self) -> Result<Expr, String> {
        let mut terms = vec![self.parse_equality_expression()?];
        while let Some(Token::Amp) = self.peek(0) {
            self.expect(Token::Amp)?; // consume '&'
            terms.push(self.parse_equality_expression()?);
        }
        if terms.len() == 1 { Ok(terms.remove(0)) } else { Ok(Expr::And { terms }) }
    }

    fn parse_equality_expression(&mut self) -> Result<Expr, String> {
        let left = self.parse_relational_expression()?;
        match self.peek(0) {
            Some(Token::EqualEqual) => {
                self.expect(Token::EqualEqual)?; // consume '=='
                let right = self.parse_relational_expression()?;
                Ok(Expr::Eq { left: Box::new(left), right: Box::new(right) })
            }
            Some(Token::NotEqual) => {
                self.expect(Token::NotEqual)?; // consume '!='
                let right = self.parse_relational_expression()?;
                Ok(Expr::Neq { left: Box::new(left), right: Box::new(right) })
            }
            _ => Ok(left),
        }
    }

    fn parse_relational_expression(&mut self) -> Result<Expr, String> {
        let left = self.parse_additive_expression()?;
        match self.peek(0) {
            Some(Token::LessThan) => {
                self.expect(Token::LessThan)?; // consume '<'
                let right = self.parse_additive_expression()?;
                Ok(Expr::Lt { left: Box::new(left), right: Box::new(right) })
            }
            Some(Token::LessEqual) => {
                self.expect(Token::LessEqual)?; // consume '<='
                let right = self.parse_additive_expression()?;
                Ok(Expr::Leq { left: Box::new(left), right: Box::new(right) })
            }
            Some(Token::GreaterThan) => {
                self.expect(Token::GreaterThan)?; // consume '>'
                let right = self.parse_additive_expression()?;
                Ok(Expr::Gt { left: Box::new(left), right: Box::new(right) })
            }
            Some(Token::GreaterEqual) => {
                self.expect(Token::GreaterEqual)?; // consume '>='
                let right = self.parse_additive_expression()?;
                Ok(Expr::Geq { left: Box::new(left), right: Box::new(right) })
            }
            _ => Ok(left),
        }
    }

    fn parse_additive_expression(&mut self) -> Result<Expr, String> {
        let mut terms = vec![self.parse_multiplicative_expression()?];
        while let Some(token) = self.peek(0) {
            match token {
                Token::Plus => {
                    self.expect(Token::Plus)?; // consume '+'
                    terms.push(self.parse_multiplicative_expression()?);
                }
                Token::Minus => {
                    self.expect(Token::Minus)?; // consume '-'
                    let right = self.parse_multiplicative_expression()?;
                    terms.push(Expr::Opposite { term: Box::new(right) });
                }
                _ => break,
            }
        }
        if terms.len() == 1 { Ok(terms.remove(0)) } else { Ok(Expr::Sum { terms }) }
    }

    fn parse_multiplicative_expression(&mut self) -> Result<Expr, String> {
        let mut factors = vec![self.parse_primary_expression()?];
        while let Some(token) = self.peek(0) {
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
                while let Some(Token::Dot) = self.peek(0) {
                    self.expect(Token::Dot)?; // consume '.'
                    if let Some(Token::Identifier(next_name)) = self.next() {
                        ids.push(next_name);
                    } else {
                        return Err("Expected identifier after '.'".to_string());
                    }
                }
                if let Some(Token::LParen) = self.peek(0) {
                    self.expect(Token::LParen)?;
                    let mut exprs = Vec::new();
                    while !matches!(self.peek(0), Some(Token::RParen)) {
                        exprs.push(self.parse_expression()?);
                        if let Some(Token::Comma) = self.peek(0) {
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

pub fn parse_expression(input: &str) -> Expr {
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_expression().expect("Failed to parse expression")
}

#[cfg(test)]
mod tests {
    use super::*;

    fn parse_primary_expression(input: &str) -> Expr {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        parser.parse_primary_expression().expect("Failed to parse primary expression")
    }

    fn parse_arithmetic_expression(input: &str) -> Expr {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        parser.parse_additive_expression().expect("Failed to parse arithmetic expression")
    }

    fn parse_equality_expression(input: &str) -> Expr {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        parser.parse_equality_expression().expect("Failed to parse equality expression")
    }

    #[test]
    fn test_primary_expressions() {
        assert_eq!(parse_primary_expression("true"), Expr::Bool(true));
        assert_eq!(parse_primary_expression("false"), Expr::Bool(false));
        assert_eq!(parse_primary_expression("123"), Expr::Int(123));
        assert_eq!(parse_primary_expression("12.34"), Expr::Real(1234, 100));
        assert_eq!(parse_primary_expression("foo"), Expr::QualifiedId { ids: vec!["foo".to_string()] });
        assert_eq!(parse_primary_expression("foo.bar"), Expr::QualifiedId { ids: vec!["foo".to_string(), "bar".to_string()] });
        assert_eq!(parse_primary_expression("(123)"), Expr::Int(123));
        assert_eq!(parse_primary_expression("f()"), Expr::Function { name: vec!["f".to_string()], args: vec![] });
        assert_eq!(parse_primary_expression("g(1, true)"), Expr::Function { name: vec!["g".to_string()], args: vec![Expr::Int(1), Expr::Bool(true)] });
        assert_eq!(parse_primary_expression("Math.max(1, 2)"), Expr::Function { name: vec!["Math".to_string(), "max".to_string()], args: vec![Expr::Int(1), Expr::Int(2)] });
    }

    #[test]
    fn test_arithmetic() {
        // 1 + 2
        assert_eq!(parse_arithmetic_expression("1 + 2"), Expr::Sum { terms: vec![Expr::Int(1), Expr::Int(2)] });

        // 1 * 2
        assert_eq!(parse_arithmetic_expression("1 * 2"), Expr::Mul { factors: vec![Expr::Int(1), Expr::Int(2)] });

        // 1 + 2 * 3
        assert_eq!(parse_arithmetic_expression("1 + 2 * 3"), Expr::Sum { terms: vec![Expr::Int(1), Expr::Mul { factors: vec![Expr::Int(2), Expr::Int(3)] },] });

        // (1 + 2) * 3
        assert_eq!(parse_arithmetic_expression("(1 + 2) * 3"), Expr::Mul { factors: vec![Expr::Sum { terms: vec![Expr::Int(1), Expr::Int(2)] }, Expr::Int(3),] });
    }

    #[test]
    fn test_relational() {
        assert_eq!(parse_equality_expression("1 < 2"), Expr::Lt { left: Box::new(Expr::Int(1)), right: Box::new(Expr::Int(2)) });
        assert_eq!(parse_equality_expression("1 <= 2"), Expr::Leq { left: Box::new(Expr::Int(1)), right: Box::new(Expr::Int(2)) });
        assert_eq!(parse_equality_expression("1 > 2"), Expr::Gt { left: Box::new(Expr::Int(1)), right: Box::new(Expr::Int(2)) });
        assert_eq!(parse_equality_expression("1 >= 2"), Expr::Geq { left: Box::new(Expr::Int(1)), right: Box::new(Expr::Int(2)) });
        assert_eq!(parse_equality_expression("1 == 1"), Expr::Eq { left: Box::new(Expr::Int(1)), right: Box::new(Expr::Int(1)) });
        assert_eq!(parse_equality_expression("1 != 2"), Expr::Neq { left: Box::new(Expr::Int(1)), right: Box::new(Expr::Int(2)) });
    }

    #[test]
    fn test_logical() {
        assert_eq!(parse_expression("true & false"), Expr::And { terms: vec![Expr::Bool(true), Expr::Bool(false)] });
        assert_eq!(parse_expression("true | false"), Expr::Or { terms: vec![Expr::Bool(true), Expr::Bool(false)] });

        // n-ary logical ops
        assert_eq!(
            parse_expression("a & b & c"),
            Expr::And {
                terms: vec![Expr::QualifiedId { ids: vec!["a".to_string()] }, Expr::QualifiedId { ids: vec!["b".to_string()] }, Expr::QualifiedId { ids: vec!["c".to_string()] },]
            }
        );

        // Mixed precedence: & binds tighter than |
        assert_eq!(
            parse_expression("a | b & c"),
            Expr::Or {
                terms: vec![
                    Expr::QualifiedId { ids: vec!["a".to_string()] },
                    Expr::And {
                        terms: vec![Expr::QualifiedId { ids: vec!["b".to_string()] }, Expr::QualifiedId { ids: vec!["c".to_string()] },]
                    }
                ]
            }
        );
    }

    #[test]
    fn test_complex_expression() {
        assert_eq!(
            parse_expression("f(x) + 3 * (y - 2) >= 10 & g(z) != 5"),
            Expr::And {
                terms: vec![
                    Expr::Geq {
                        left: Box::new(Expr::Sum {
                            terms: vec![
                                Expr::Function { name: vec!["f".to_string()], args: vec![Expr::QualifiedId { ids: vec!["x".to_string()] }] },
                                Expr::Mul {
                                    factors: vec![
                                        Expr::Int(3),
                                        Expr::Sum {
                                            terms: vec![Expr::QualifiedId { ids: vec!["y".to_string()] }, Expr::Opposite { term: Box::new(Expr::Int(2)) },]
                                        }
                                    ]
                                }
                            ]
                        }),
                        right: Box::new(Expr::Int(10))
                    },
                    Expr::Neq {
                        left: Box::new(Expr::Function { name: vec!["g".to_string()], args: vec![Expr::QualifiedId { ids: vec!["z".to_string()] }] }),
                        right: Box::new(Expr::Int(5))
                    }
                ]
            }
        );
    }
}
