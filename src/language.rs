pub struct CompilationUnit {
    methods: Vec<Method>,
    predicates: Vec<Predicate>,
    classes: Vec<Class>,
    statements: Vec<Statement>,
}

pub struct Class {
    name: String,
    parents: Vec<Vec<String>>,
    fields: Vec<(Vec<String>, Vec<String>)>,
    constructors: Vec<Constructor>,
    methods: Vec<Method>,
    predicates: Vec<Predicate>,
}

pub struct Constructor {
    args: Vec<(Vec<String>, String)>,
    init: Vec<(String, Vec<Expr>)>,
    statements: Vec<Statement>,
}

pub struct Method {
    return_type: Option<Vec<String>>,
    name: String,
    args: Vec<(Vec<String>, String)>,
    statements: Vec<Statement>,
}

pub struct Predicate {
    name: String,
    args: Vec<(Vec<String>, String)>,
    statements: Vec<Statement>,
}

#[derive(Debug, PartialEq)]
pub enum Expr {
    Bool(bool),
    Int(i64),
    Real(i64, i64),
    QualifiedId { ids: Vec<String> },
    Sum { terms: Vec<Expr> },
    Opposite { term: Box<Expr> },
    Mul { factors: Vec<Expr> },
    Div { left: Box<Expr>, right: Box<Expr> },
    Function { name: Vec<String>, args: Vec<Expr> },
    Eq { left: Box<Expr>, right: Box<Expr> },
    Neq { left: Box<Expr>, right: Box<Expr> },
    Lt { left: Box<Expr>, right: Box<Expr> },
    Leq { left: Box<Expr>, right: Box<Expr> },
    Gt { left: Box<Expr>, right: Box<Expr> },
    Geq { left: Box<Expr>, right: Box<Expr> },
    Or { terms: Vec<Expr> },
    And { terms: Vec<Expr> },
}

#[derive(Debug, PartialEq)]
pub enum Statement {
    Expr(Expr),
    LocalField { field_type: Vec<String>, fields: Vec<(String, Option<Expr>)> },
    Assign { name: Vec<String>, value: Expr },
    ForAll { var_type: Vec<String>, var_name: String, statements: Vec<Statement> },
    Disjunction { disjuncts: Vec<(Vec<Statement>, Expr)> },
    Formula { is_fact: bool, name: String, predicate_name: Vec<String>, args: Vec<(String, Expr)> },
    Return { value: Expr },
}
