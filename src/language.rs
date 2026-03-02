use std::fmt::{Display, Formatter, Result};

#[derive(Debug, PartialEq)]
pub struct ProblemDef {
    pub methods: Vec<MethodDef>,
    pub predicates: Vec<PredicateDef>,
    pub classes: Vec<ClassDef>,
    pub enums: Vec<EnumDef>,
    pub statements: Vec<Statement>,
}

pub type Field = (Vec<String>, Vec<(String, Option<Expr>)>); // (type, [(name, optional initializer)])

#[derive(Debug, PartialEq)]
pub struct EnumDef {
    pub name: String,
    pub values: Vec<String>,
}

#[derive(Debug, PartialEq)]
pub struct ClassDef {
    pub name: String,
    pub parents: Vec<Vec<String>>,
    pub fields: Vec<Field>,
    pub constructors: Vec<ConstructorDef>,
    pub methods: Vec<MethodDef>,
    pub predicates: Vec<PredicateDef>,
    pub classes: Vec<ClassDef>,
    pub enums: Vec<EnumDef>,
}

#[derive(Debug, PartialEq)]
pub struct ConstructorDef {
    pub args: Vec<(Vec<String>, String)>,
    pub init: Vec<(String, Vec<Expr>)>,
    pub statements: Vec<Statement>,
}

#[derive(Debug, PartialEq)]
pub struct MethodDef {
    pub return_type: Option<Vec<String>>,
    pub name: String,
    pub args: Vec<(Vec<String>, String)>,
    pub statements: Vec<Statement>,
}

#[derive(Debug, PartialEq)]
pub struct PredicateDef {
    pub name: String,
    pub args: Vec<(Vec<String>, String)>,
    pub parents: Vec<Vec<String>>,
    pub statements: Vec<Statement>,
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
    String(String),
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
    NewObject { class_name: Vec<String>, args: Vec<Expr> },
}

impl Display for ProblemDef {
    fn fmt(&self, f: &mut Formatter<'_>) -> Result {
        for method in &self.methods {
            writeln!(f, "{}", method)?;
        }
        for predicate in &self.predicates {
            writeln!(f, "{}", predicate)?;
        }
        for class in &self.classes {
            writeln!(f, "{}", class)?;
        }
        for statement in &self.statements {
            writeln!(f, "{}", statement)?;
        }
        Ok(())
    }
}

impl Display for ClassDef {
    fn fmt(&self, f: &mut Formatter<'_>) -> Result {
        writeln!(f, "class {}{} {{", self.name, if !self.parents.is_empty() { format!(" extends {}", self.parents.iter().map(|p| p.join(".")).collect::<Vec<_>>().join(", ")) } else { String::new() })?;
        for (field_type, fields) in &self.fields {
            writeln!(f, "    {} {};", field_type.join("."), fields.iter().map(|(n, v)| format!("{}{}", n, v.as_ref().map(|v| format!(" = {}", v)).unwrap_or_default())).collect::<Vec<_>>().join(", "))?;
        }
        for constructor in &self.constructors {
            writeln!(f, "    {}", constructor)?;
        }
        for method in &self.methods {
            writeln!(f, "    {}", method)?;
        }
        for predicate in &self.predicates {
            writeln!(f, "    {}", predicate)?;
        }
        write!(f, "}}")
    }
}

impl Display for ConstructorDef {
    fn fmt(&self, f: &mut Formatter<'_>) -> Result {
        write!(f, "constructor({}) {{\n{}\n}}", self.args.iter().map(|(t, n)| format!("{} {}", t.join("."), n)).collect::<Vec<_>>().join(", "), self.statements.iter().map(|s| format!("    {}", s)).collect::<Vec<_>>().join("\n"))
    }
}

impl Display for MethodDef {
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

impl Display for PredicateDef {
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
            Expr::String(s) => write!(f, "\"{}\"", s),
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
            Expr::NewObject { class_name, args } => write!(f, "new {}({})", class_name.join("."), args.iter().map(|a| format!("{}", a)).collect::<Vec<_>>().join(", ")),
        }
    }
}
