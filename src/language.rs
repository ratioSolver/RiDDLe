use std::fmt::{Display, Formatter, Result};

#[derive(Debug, PartialEq)]
pub struct CompilationUnit {
    methods: Vec<Method>,
    predicates: Vec<Predicate>,
    classes: Vec<Class>,
    statements: Vec<Statement>,
}

#[derive(Debug, PartialEq)]
pub struct Class {
    name: String,
    parents: Vec<Vec<String>>,
    fields: Vec<(Vec<String>, Vec<(String, Option<Expr>)>)>,
    constructors: Vec<Constructor>,
    methods: Vec<Method>,
    predicates: Vec<Predicate>,
}

#[derive(Debug, PartialEq)]
pub struct Constructor {
    args: Vec<(Vec<String>, String)>,
    init: Vec<(String, Vec<Expr>)>,
    statements: Vec<Statement>,
}

#[derive(Debug, PartialEq)]
pub struct Method {
    return_type: Option<Vec<String>>,
    name: String,
    args: Vec<(Vec<String>, String)>,
    statements: Vec<Statement>,
}

#[derive(Debug, PartialEq)]
pub struct Predicate {
    name: String,
    args: Vec<(Vec<String>, String)>,
    statements: Vec<Statement>,
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

impl Display for CompilationUnit {
    fn fmt(&self, f: &mut Formatter<'_>) -> Result {
        for method in &self.methods {
            write!(f, "{}\n", method)?;
        }
        for predicate in &self.predicates {
            write!(f, "{}\n", predicate)?;
        }
        for class in &self.classes {
            write!(f, "{}\n", class)?;
        }
        for statement in &self.statements {
            write!(f, "{}\n", statement)?;
        }
        Ok(())
    }
}

impl Display for Class {
    fn fmt(&self, f: &mut Formatter<'_>) -> Result {
        write!(f, "class {}{} {{\n", self.name, if !self.parents.is_empty() { format!(" extends {}", self.parents.iter().map(|p| p.join(".")).collect::<Vec<_>>().join(", ")) } else { String::new() })?;
        for (field_type, fields) in &self.fields {
            write!(f, "    {} {};\n", field_type.join("."), fields.iter().map(|(n, v)| format!("{}{}", n, v.as_ref().map(|v| format!(" = {}", v)).unwrap_or_default())).collect::<Vec<_>>().join(", "))?;
        }
        for constructor in &self.constructors {
            write!(f, "    {}\n", constructor)?;
        }
        for method in &self.methods {
            write!(f, "    {}\n", method)?;
        }
        for predicate in &self.predicates {
            write!(f, "    {}\n", predicate)?;
        }
        write!(f, "}}")
    }
}

impl Display for Constructor {
    fn fmt(&self, f: &mut Formatter<'_>) -> Result {
        write!(f, "constructor({}) {{\n{}\n}}", self.args.iter().map(|(t, n)| format!("{} {}", t.join("."), n)).collect::<Vec<_>>().join(", "), self.statements.iter().map(|s| format!("    {}", s)).collect::<Vec<_>>().join("\n"))
    }
}

impl Display for Method {
    fn fmt(&self, f: &mut Formatter<'_>) -> Result {
        write!(
            f,
            "{} {}({}) {{\n{}\n}}",
            self.return_type.as_ref().map(|t| t.join(".")).unwrap_or_else(|| "void".to_string()),
            self.name,
            self.args.iter().map(|(t, n)| format!("{} {}", t.join("."), n)).collect::<Vec<_>>().join(", "),
            self.statements.iter().map(|s| format!("    {}", s)).collect::<Vec<_>>().join("\n")
        )
    }
}

impl Display for Predicate {
    fn fmt(&self, f: &mut Formatter<'_>) -> Result {
        write!(f, "predicate {}({}) {{\n{}\n}}", self.name, self.args.iter().map(|(t, n)| format!("{} {}", t.join("."), n)).collect::<Vec<_>>().join(", "), self.statements.iter().map(|s| format!("    {}", s)).collect::<Vec<_>>().join("\n"))
    }
}

impl Display for Statement {
    fn fmt(&self, f: &mut Formatter<'_>) -> Result {
        match self {
            Statement::Expr(e) => write!(f, "{};", e),
            Statement::LocalField { field_type, fields } => write!(f, "{} {};", field_type.join("."), fields.iter().map(|(n, v)| format!("{}{}", n, v.as_ref().map(|v| format!(" = {}", v)).unwrap_or_default())).collect::<Vec<_>>().join(", ")),
            Statement::Assign { name, value } => write!(f, "{} = {};", name.join("."), value),
            Statement::ForAll { var_type, var_name, statements } => write!(f, "for {} {} {{\n{}\n}}", var_type.join("."), var_name, statements.iter().map(|s| format!("    {}", s)).collect::<Vec<_>>().join("\n")),
            Statement::Disjunction { disjuncts } => write!(f, "{{\n{}\n}}", disjuncts.iter().map(|(s, e)| format!("    {{\n{}\n    }}: {}", s.iter().map(|s| format!("        {}", s)).collect::<Vec<_>>().join("\n"), e)).collect::<Vec<_>>().join(" or ")),
            Statement::Formula { is_fact, name, predicate_name, args } => write!(f, "{} {}({}): {}({});", if *is_fact { "fact" } else { "goal" }, name, predicate_name.join("."), predicate_name.join("."), args.iter().map(|(n, v)| format!("{} = {}", n, v)).collect::<Vec<_>>().join(", ")),
            Statement::Return { value } => write!(f, "return {};", value),
        }
    }
}

impl Display for Expr {
    fn fmt(&self, f: &mut Formatter<'_>) -> Result {
        match self {
            Expr::Bool(b) => write!(f, "{}", b),
            Expr::Int(i) => write!(f, "{}", i),
            Expr::Real(n, d) => write!(f, "{}/{}", n, d),
            Expr::QualifiedId { ids } => write!(f, "{}", ids.join(".")),
            Expr::Sum { terms } => write!(f, "({})", terms.iter().map(|t| format!("{}", t)).collect::<Vec<_>>().join(" + ")),
            Expr::Opposite { term } => write!(f, "-({})", term),
            Expr::Mul { factors } => write!(f, "({})", factors.iter().map(|t| format!("{}", t)).collect::<Vec<_>>().join(" * ")),
            Expr::Div { left, right } => write!(f, "({} / {})", left, right),
            Expr::Function { name, args } => write!(f, "{}({})", name.join("."), args.iter().map(|a| format!("{}", a)).collect::<Vec<_>>().join(", ")),
            Expr::Eq { left, right } => write!(f, "({} == {})", left, right),
            Expr::Neq { left, right } => write!(f, "({} != {})", left, right),
            Expr::Lt { left, right } => write!(f, "({} < {})", left, right),
            Expr::Leq { left, right } => write!(f, "({} <= {})", left, right),
            Expr::Gt { left, right } => write!(f, "({} > {})", left, right),
            Expr::Geq { left, right } => write!(f, "({} >= {})", left, right),
            Expr::Or { terms } => write!(f, "({})", terms.iter().map(|t| format!("{}", t)).collect::<Vec<_>>().join(" || ")),
            Expr::And { terms } => write!(f, "({})", terms.iter().map(|t| format!("{}", t)).collect::<Vec<_>>().join(" && ")),
        }
    }
}
