use crate::language::{ClassDef, ConstructorDef, EnumDef, Expr, MethodDef, PredicateDef, ProblemDef, Statement};
use std::{
    any::Any,
    cell::RefCell,
    collections::HashMap,
    rc::{Rc, Weak},
};

pub trait Type {
    fn name(&self) -> &str;
    fn full_name(&self) -> &str {
        self.name()
    }
    fn as_any(self: Rc<Self>) -> Rc<dyn Any>;
    fn as_class(self: Rc<Self>) -> Option<Rc<dyn Class>> {
        None
    }
    fn new_instance(self: Rc<Self>) -> Rc<dyn Var>;
}

pub struct BoolType {
    core: Weak<dyn Core>,
}

impl BoolType {
    pub fn new(core: Weak<dyn Core>) -> Self {
        Self { core }
    }
}

impl Type for BoolType {
    fn name(&self) -> &str {
        "bool"
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn new_instance(self: Rc<Self>) -> Rc<dyn Var> {
        self.core.upgrade().unwrap().new_bool_var()
    }
}

pub struct IntType {
    core: Weak<dyn Core>,
}

impl IntType {
    pub fn new(core: Weak<dyn Core>) -> Self {
        Self { core }
    }
}

impl Type for IntType {
    fn name(&self) -> &str {
        "int"
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn new_instance(self: Rc<Self>) -> Rc<dyn Var> {
        self.core.upgrade().unwrap().new_int_var()
    }
}

pub struct RealType {
    core: Weak<dyn Core>,
}

impl RealType {
    pub fn new(core: Weak<dyn Core>) -> Self {
        Self { core }
    }
}

impl Type for RealType {
    fn name(&self) -> &str {
        "real"
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn new_instance(self: Rc<Self>) -> Rc<dyn Var> {
        self.core.upgrade().unwrap().new_real_var()
    }
}

pub struct StringType {
    core: Weak<dyn Core>,
}

impl StringType {
    pub fn new(core: Weak<dyn Core>) -> Self {
        Self { core }
    }
}

impl Type for StringType {
    fn name(&self) -> &str {
        "string"
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn new_instance(self: Rc<Self>) -> Rc<dyn Var> {
        self.core.upgrade().unwrap().new_string_var()
    }
}

pub struct Field {
    name: String,
    field_type: Vec<String>,
    default: Option<Expr>,
}

impl Field {
    pub fn new(name: String, field_type: Vec<String>, default: Option<Expr>) -> Self {
        Self { name, field_type, default }
    }

    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn field_type(&self) -> &[String] {
        &self.field_type
    }

    pub fn default(&self) -> Option<&Expr> {
        self.default.as_ref()
    }
}

pub trait Var {
    fn class(&self) -> Rc<dyn Type>;
    fn as_any(self: Rc<Self>) -> Rc<dyn Any>;
    fn as_env(&self) -> Option<&dyn Env> {
        None
    }
}

pub trait Scope {
    fn as_class(self: Rc<Self>) -> Option<Rc<dyn Class>> {
        None
    }
    fn core(self: Rc<Self>) -> Rc<dyn Core>;
    fn parent(&self) -> Option<Rc<dyn Scope>>;

    fn get_field(&self, name: &str) -> Option<Rc<Field>>;
    fn get_method(&self, name: &str, types: &[Rc<dyn Type>]) -> Option<Rc<Method>>;
    fn get_class(&self, name: &str) -> Option<Rc<dyn Type>>;
    fn get_enum(&self, name: &str) -> Option<Rc<EnumDef>>;
    fn get_predicate(&self, name: &str) -> Option<Rc<PredicateDef>>;
}

pub trait Env {
    fn parent(&self) -> Option<Rc<dyn Env>>;
    fn get(&self, name: &str) -> Option<Rc<dyn Var>>;
    fn set(&self, name: String, value: Rc<dyn Var>);
}

pub trait EnvExt {
    fn get_as<T: Var + 'static>(&self, name: &str) -> Option<Rc<T>>;
}

impl<E: Env + ?Sized> EnvExt for E {
    fn get_as<T: Var + 'static>(&self, name: &str) -> Option<Rc<T>> {
        self.get(name)?.as_any().downcast::<T>().ok()
    }
}

pub struct CommonEnv {
    parent: Option<Rc<dyn Env>>,
    variables: RefCell<HashMap<String, Rc<dyn Var>>>,
}

pub struct CommonScope {
    core: Weak<dyn Core>,
    parent: Option<Rc<dyn Scope>>,
    fields: RefCell<HashMap<String, Rc<Field>>>,
    methods: RefCell<HashMap<String, Vec<Rc<Method>>>>,
    classes: RefCell<HashMap<String, Rc<dyn Type>>>,
    enums: RefCell<HashMap<String, Rc<EnumDef>>>,
    predicates: RefCell<HashMap<String, Rc<PredicateDef>>>,
}

impl CommonEnv {
    pub fn new(parent: Option<Rc<dyn Env>>) -> Self {
        Self { parent, variables: RefCell::new(HashMap::new()) }
    }
}

impl Env for CommonEnv {
    fn parent(&self) -> Option<Rc<dyn Env>> {
        self.parent.clone()
    }

    fn get(&self, name: &str) -> Option<Rc<dyn Var>> {
        self.variables.borrow().get(name).cloned().or_else(|| self.parent.as_ref()?.get(name))
    }

    fn set(&self, name: String, value: Rc<dyn Var>) {
        self.variables.borrow_mut().insert(name, value);
    }
}

impl CommonScope {
    pub fn new(core: Weak<dyn Core>, parent: Option<Rc<dyn Scope>>) -> Self {
        Self {
            core,
            parent,
            fields: RefCell::new(HashMap::new()),
            methods: RefCell::new(HashMap::new()),
            classes: RefCell::new(HashMap::new()),
            enums: RefCell::new(HashMap::new()),
            predicates: RefCell::new(HashMap::new()),
        }
    }

    pub fn from_class(core: Weak<dyn Core>, parent: Option<Rc<dyn Scope>>, class: ClassDef) -> Rc<Self> {
        let scope = Rc::new(Self::new(core.clone(), parent));
        for (field_type, fields) in class.fields {
            for (name, default) in fields {
                scope.fields.borrow_mut().insert(name.clone(), Rc::new(Field { name, field_type: field_type.clone(), default }));
            }
        }
        for method in class.methods {
            scope.methods.borrow_mut().entry(method.name.clone()).or_default().push(Rc::new(Method::new(core.clone(), Some(scope.clone()), method)));
        }
        for predicate in class.predicates {
            scope.predicates.borrow_mut().insert(predicate.name.clone(), Rc::new(predicate));
        }
        scope
    }

    pub fn from_costructor(core: Weak<dyn Core>, parent: Option<Rc<dyn Scope>>, constructor: ConstructorDef) -> Self {
        let scope = Self::new(core, parent);
        for (arg_type, arg_name) in constructor.args {
            scope.fields.borrow_mut().insert(arg_name.clone(), Rc::new(Field { name: arg_name, field_type: arg_type, default: None }));
        }
        scope
    }

    pub fn from_method(core: Weak<dyn Core>, parent: Option<Rc<dyn Scope>>, method: MethodDef) -> Self {
        let scope = Self::new(core, parent);
        for (arg_type, arg_name) in method.args {
            scope.fields.borrow_mut().insert(arg_name.clone(), Rc::new(Field { name: arg_name, field_type: arg_type, default: None }));
        }
        scope
    }

    pub fn from_predicate(core: Weak<dyn Core>, parent: Option<Rc<dyn Scope>>, predicate: PredicateDef) -> Self {
        let scope = Self::new(core, parent);
        for (arg_type, arg_name) in predicate.args {
            scope.fields.borrow_mut().insert(arg_name.clone(), Rc::new(Field { name: arg_name, field_type: arg_type, default: None }));
        }
        scope
    }

    pub fn add_problem(&self, problem: ProblemDef) {
        for method in problem.methods {
            self.methods.borrow_mut().entry(method.name.clone()).or_default().push(Rc::new(Method::new(self.core.clone(), Some(self.core.upgrade().unwrap()), method)));
        }
        for predicate in problem.predicates {
            self.predicates.borrow_mut().insert(predicate.name.clone(), Rc::new(predicate));
        }
        for class in problem.classes {
            self.classes.borrow_mut().insert(class.name.clone(), Rc::new(CommonClass::new(self.core.clone(), Some(self.core.upgrade().unwrap()), class)));
        }
        for enm in problem.enums {
            self.enums.borrow_mut().insert(enm.name.clone(), Rc::new(enm));
        }
    }
}

impl Scope for CommonScope {
    fn core(self: Rc<Self>) -> Rc<dyn Core> {
        self.core.upgrade().unwrap()
    }

    fn parent(&self) -> Option<Rc<dyn Scope>> {
        self.parent.clone()
    }

    fn get_field(&self, name: &str) -> Option<Rc<Field>> {
        self.fields.borrow().get(name).cloned().or_else(|| self.parent.as_ref()?.get_field(name))
    }

    fn get_method(&self, name: &str, types: &[Rc<dyn Type>]) -> Option<Rc<Method>> {
        self.methods
            .borrow()
            .get(name)
            .and_then(|methods| {
                methods
                    .iter()
                    .find(|m| {
                        if m.args().len() != types.len() {
                            return false;
                        }
                        for (class, arg_type) in types.iter().zip(m.args().iter().map(|(t, _)| t)) {
                            if !class.full_name().split('.').eq(arg_type.iter().map(|s| s.as_str())) {
                                return false;
                            }
                        }
                        true
                    })
                    .cloned()
            })
            .or_else(|| self.parent.as_ref()?.get_method(name, types))
    }

    fn get_class(&self, name: &str) -> Option<Rc<dyn Type>> {
        self.classes.borrow().get(name).cloned().or_else(|| self.parent.as_ref()?.get_class(name))
    }

    fn get_enum(&self, name: &str) -> Option<Rc<EnumDef>> {
        self.enums.borrow().get(name).cloned().or_else(|| self.parent.as_ref()?.get_enum(name))
    }

    fn get_predicate(&self, name: &str) -> Option<Rc<PredicateDef>> {
        self.predicates.borrow().get(name).cloned().or_else(|| self.parent.as_ref()?.get_predicate(name))
    }
}

pub struct Method {
    core: Weak<dyn Core>,
    scope: Rc<CommonScope>,
    name: String,
    return_type: Option<Vec<String>>,
    args: Vec<(Vec<String>, String)>,
    statements: Vec<Statement>,
}

impl Method {
    pub fn new(core: Weak<dyn Core>, parent: Option<Rc<dyn Scope>>, mut method: MethodDef) -> Self {
        Self {
            core: core.clone(),
            name: std::mem::take(&mut method.name),
            return_type: std::mem::take(&mut method.return_type),
            args: std::mem::take(&mut method.args),
            statements: std::mem::take(&mut method.statements),
            scope: Rc::new(CommonScope::from_method(core, parent, method)),
        }
    }

    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn return_type(&self) -> Option<&[String]> {
        self.return_type.as_deref()
    }

    pub fn args(&self) -> &[(Vec<String>, String)] {
        &self.args
    }

    pub fn statements(&self) -> &[Statement] {
        &self.statements
    }

    pub fn call(&self, env: Rc<dyn Env>, args: Vec<Rc<dyn Var>>) -> Result<Option<Rc<dyn Var>>, RiddleError> {
        if args.len() != self.args.len() {
            return Err(RiddleError::RuntimeError(format!("Expected {} arguments, got {}", self.args.len(), args.len())));
        }
        let method_env = Rc::new(CommonEnv::new(Some(env)));
        for ((arg_type, arg_name), arg_value) in self.args.iter().zip(args.into_iter()) {
            if !arg_value.class().full_name().split('.').eq(arg_type.iter().map(|s| s.as_str())) {
                return Err(RiddleError::TypeError(format!("Argument '{}' expected to be of type '{}', got '{}'", arg_name, arg_type.join("."), arg_value.class().name())));
            }
            method_env.set(arg_name.clone(), arg_value);
        }
        for stmt in &self.statements {
            execute(self.scope.clone(), method_env.clone(), stmt)?;
        }
        if let Some(return_type) = &self.return_type {
            method_env
                .get("return")
                .ok_or_else(|| RiddleError::RuntimeError("Method did not set return value".into()))
                .and_then(|ret| if ret.class().full_name().split('.').eq(return_type.iter().map(|s| s.as_str())) { Ok(Some(ret)) } else { Err(RiddleError::TypeError(format!("Return value expected to be of type '{}', got '{}'", return_type.join("."), ret.class().name()))) })
        } else {
            Ok(None)
        }
    }
}

impl Scope for Method {
    fn core(self: Rc<Self>) -> Rc<dyn Core> {
        self.core.upgrade().unwrap()
    }

    fn parent(&self) -> Option<Rc<dyn Scope>> {
        self.scope.parent.clone()
    }

    fn get_field(&self, name: &str) -> Option<Rc<Field>> {
        self.scope.get_field(name)
    }

    fn get_method(&self, name: &str, classes: &[Rc<dyn Type>]) -> Option<Rc<Method>> {
        self.scope.get_method(name, classes)
    }

    fn get_class(&self, name: &str) -> Option<Rc<dyn Type>> {
        self.scope.get_class(name)
    }

    fn get_enum(&self, name: &str) -> Option<Rc<EnumDef>> {
        self.scope.get_enum(name)
    }

    fn get_predicate(&self, name: &str) -> Option<Rc<PredicateDef>> {
        self.scope.get_predicate(name)
    }
}

pub struct Constructor {
    core: Weak<dyn Core>,
    scope: Rc<CommonScope>,
    args: Vec<(Vec<String>, String)>,
    statements: Vec<Statement>,
}

impl Constructor {
    pub fn new(core: Weak<dyn Core>, parent: Option<Rc<dyn Scope>>, mut constructor: ConstructorDef) -> Self {
        Self {
            core: core.clone(),
            args: std::mem::take(&mut constructor.args),
            statements: std::mem::take(&mut constructor.statements),
            scope: Rc::new(CommonScope::from_costructor(core, parent, constructor)),
        }
    }

    pub fn args(&self) -> &[(Vec<String>, String)] {
        &self.args
    }

    pub fn statements(&self) -> &[Statement] {
        &self.statements
    }

    pub fn call(&self, env: Rc<dyn Env>, args: Vec<Rc<dyn Var>>) -> Result<Option<Rc<dyn Var>>, RiddleError> {
        if args.len() != self.args.len() {
            return Err(RiddleError::RuntimeError(format!("Expected {} arguments, got {}", self.args.len(), args.len())));
        }
        let class = self.scope.parent.as_ref().expect("Constructor scope should have a parent").clone().as_class().expect("Constructor scope parent should be a class");
        let object = class.new_instance();
        let constructor_env = Rc::new(CommonEnv::new(Some(env)));
        constructor_env.set("this".to_string(), object.clone());
        for ((arg_type, arg_name), arg_value) in self.args.iter().zip(args.into_iter()) {
            if !arg_value.class().full_name().split('.').eq(arg_type.iter().map(|s| s.as_str())) {
                return Err(RiddleError::TypeError(format!("Argument '{}' expected to be of type '{}', got '{}'", arg_name, arg_type.join("."), arg_value.class().name())));
            }
            constructor_env.set(arg_name.clone(), arg_value);
        }
        for stmt in &self.statements {
            execute(self.scope.clone(), constructor_env.clone(), stmt)?;
        }
        Ok(Some(object))
    }
}

impl Scope for Constructor {
    fn core(self: Rc<Self>) -> Rc<dyn Core> {
        self.core.upgrade().unwrap()
    }

    fn parent(&self) -> Option<Rc<dyn Scope>> {
        self.scope.parent.clone()
    }

    fn get_field(&self, name: &str) -> Option<Rc<Field>> {
        self.scope.get_field(name)
    }

    fn get_method(&self, name: &str, classes: &[Rc<dyn Type>]) -> Option<Rc<Method>> {
        self.scope.get_method(name, classes)
    }

    fn get_class(&self, name: &str) -> Option<Rc<dyn Type>> {
        self.scope.get_class(name)
    }

    fn get_enum(&self, name: &str) -> Option<Rc<EnumDef>> {
        self.scope.get_enum(name)
    }

    fn get_predicate(&self, name: &str) -> Option<Rc<PredicateDef>> {
        self.scope.get_predicate(name)
    }
}

pub struct Predicate {
    core: Weak<dyn Core>,
    scope: CommonScope,
    name: String,
    args: Vec<(Vec<String>, String)>,
    statements: Vec<Statement>,
    atoms: RefCell<Vec<Rc<Atom>>>,
}

impl Predicate {
    pub fn new(core: Weak<dyn Core>, parent: Option<Rc<dyn Scope>>, mut predicate: PredicateDef) -> Self {
        Self {
            core: core.clone(),
            name: std::mem::take(&mut predicate.name),
            args: std::mem::take(&mut predicate.args),
            statements: std::mem::take(&mut predicate.statements),
            scope: CommonScope::from_predicate(core, parent, predicate),
            atoms: RefCell::new(Vec::new()),
        }
    }

    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn args(&self) -> &[(Vec<String>, String)] {
        &self.args
    }

    pub fn statements(&self) -> &[Statement] {
        &self.statements
    }

    pub fn new_atom(self: Rc<Self>, fact: bool, parent_env: Option<Rc<dyn Env>>) -> Rc<Atom> {
        let atom = Rc::new(Atom::new(self.clone(), fact, parent_env));
        self.atoms.borrow_mut().push(atom.clone());
        atom
    }
}

impl Scope for Predicate {
    fn core(self: Rc<Self>) -> Rc<dyn Core> {
        self.core.upgrade().unwrap()
    }

    fn parent(&self) -> Option<Rc<dyn Scope>> {
        self.scope.parent.clone()
    }

    fn get_field(&self, name: &str) -> Option<Rc<Field>> {
        self.scope.get_field(name)
    }

    fn get_method(&self, name: &str, classes: &[Rc<dyn Type>]) -> Option<Rc<Method>> {
        self.scope.get_method(name, classes)
    }

    fn get_class(&self, name: &str) -> Option<Rc<dyn Type>> {
        self.scope.get_class(name)
    }

    fn get_enum(&self, name: &str) -> Option<Rc<EnumDef>> {
        self.scope.get_enum(name)
    }

    fn get_predicate(&self, name: &str) -> Option<Rc<PredicateDef>> {
        self.scope.get_predicate(name)
    }
}

pub struct Atom {
    predicate: Weak<Predicate>,
    fact: bool,
    env: CommonEnv,
}

impl Atom {
    pub fn new(predicate: Rc<Predicate>, fact: bool, parent_env: Option<Rc<dyn Env>>) -> Self {
        Self { predicate: Rc::downgrade(&predicate), fact, env: CommonEnv::new(parent_env) }
    }

    pub fn predicate(&self) -> Rc<Predicate> {
        self.predicate.upgrade().unwrap()
    }

    pub fn is_fact(&self) -> bool {
        self.fact
    }
}

impl Env for Atom {
    fn parent(&self) -> Option<Rc<dyn Env>> {
        self.env.parent.clone()
    }

    fn get(&self, name: &str) -> Option<Rc<dyn Var>> {
        self.env.get(name)
    }

    fn set(&self, name: String, value: Rc<dyn Var>) {
        self.env.set(name, value);
    }
}

pub trait Class: Type + Scope {
    fn parents(&self) -> &[Vec<String>];
    fn constructors(&self) -> &[Constructor];
    fn constructor(&self, args: &[Rc<dyn Type>]) -> Option<&Constructor>;
    fn instances(&self) -> Vec<Rc<Object>>;
}

pub struct CommonClass {
    core: Weak<dyn Core>,
    scope: Rc<CommonScope>,
    name: String,
    parents: Vec<Vec<String>>,
    constructors: Vec<Constructor>,
    instances: RefCell<Vec<Rc<Object>>>,
}

impl CommonClass {
    pub fn new(core: Weak<dyn Core>, parent: Option<Rc<dyn Scope>>, mut class: ClassDef) -> Self {
        Self {
            core: core.clone(),
            name: std::mem::take(&mut class.name),
            parents: std::mem::take(&mut class.parents),
            constructors: std::mem::take(&mut class.constructors).into_iter().map(|c| Constructor::new(core.clone(), parent.clone(), c)).collect(),
            scope: CommonScope::from_class(core, parent, class),
            instances: RefCell::new(Vec::new()),
        }
    }
}

impl Type for CommonClass {
    fn name(&self) -> &str {
        &self.name
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn as_class(self: Rc<Self>) -> Option<Rc<dyn Class>> {
        Some(self)
    }

    fn new_instance(self: Rc<Self>) -> Rc<dyn Var> {
        let instance = Rc::new(Object::new(self.clone(), None));
        self.instances.borrow_mut().push(instance.clone());
        instance
    }
}

impl Scope for CommonClass {
    fn core(self: Rc<Self>) -> Rc<dyn Core> {
        self.core.upgrade().unwrap()
    }

    fn parent(&self) -> Option<Rc<dyn Scope>> {
        self.scope.parent.clone()
    }

    fn get_field(&self, name: &str) -> Option<Rc<Field>> {
        self.scope.get_field(name)
    }

    fn get_method(&self, name: &str, classes: &[Rc<dyn Type>]) -> Option<Rc<Method>> {
        self.scope.get_method(name, classes)
    }

    fn get_class(&self, name: &str) -> Option<Rc<dyn Type>> {
        self.scope.get_class(name)
    }

    fn get_enum(&self, name: &str) -> Option<Rc<EnumDef>> {
        self.scope.get_enum(name)
    }

    fn get_predicate(&self, name: &str) -> Option<Rc<PredicateDef>> {
        self.scope.get_predicate(name)
    }
}

impl Class for CommonClass {
    fn parents(&self) -> &[Vec<String>] {
        &self.parents
    }

    fn constructors(&self) -> &[Constructor] {
        &self.constructors
    }

    fn constructor(&self, args: &[Rc<dyn Type>]) -> Option<&Constructor> {
        self.constructors.iter().find(|c| {
            if c.args().len() != args.len() {
                return false;
            }
            for ((arg_type, _), class) in c.args().iter().zip(args.iter()) {
                if !class.full_name().split('.').eq(arg_type.iter().map(|s| s.as_str())) {
                    return false;
                }
            }
            true
        })
    }

    fn instances(&self) -> Vec<Rc<Object>> {
        self.instances.borrow().clone()
    }
}

pub struct Object {
    class: Weak<dyn Type>,
    env: CommonEnv,
}

impl Object {
    pub fn new(class: Rc<dyn Type>, parent_env: Option<Rc<dyn Env>>) -> Self {
        Self { class: Rc::downgrade(&class), env: CommonEnv::new(parent_env) }
    }
}

impl Var for Object {
    fn class(&self) -> Rc<dyn Type> {
        self.class.upgrade().unwrap()
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn as_env(&self) -> Option<&dyn Env> {
        Some(&self.env)
    }
}

impl Env for Object {
    fn parent(&self) -> Option<Rc<dyn Env>> {
        self.env.parent.clone()
    }

    fn get(&self, name: &str) -> Option<Rc<dyn Var>> {
        self.env.get(name)
    }

    fn set(&self, name: String, value: Rc<dyn Var>) {
        self.env.set(name, value);
    }
}

pub trait Core: Scope + Env {
    fn new_bool(&self, value: bool) -> Rc<dyn Var>;
    fn new_bool_var(&self) -> Rc<dyn Var>;
    fn new_int(&self, value: i64) -> Rc<dyn Var>;
    fn new_int_var(&self) -> Rc<dyn Var>;
    fn new_real(&self, num: i64, den: i64) -> Rc<dyn Var>;
    fn new_real_var(&self) -> Rc<dyn Var>;
    fn new_string(&self, value: &str) -> Rc<dyn Var>;
    fn new_string_var(&self) -> Rc<dyn Var>;

    fn sum(&self, sum: &[Rc<dyn Var>]) -> Rc<dyn Var>;
    fn opposite(&self, term: Rc<dyn Var>) -> Rc<dyn Var>;
    fn mul(&self, mul: &[Rc<dyn Var>]) -> Rc<dyn Var>;
    fn div(&self, left: Rc<dyn Var>, right: Rc<dyn Var>) -> Rc<dyn Var>;

    fn eq(&self, left: Rc<dyn Var>, right: Rc<dyn Var>) -> Rc<dyn Var>;
    fn neq(&self, left: Rc<dyn Var>, right: Rc<dyn Var>) -> Rc<dyn Var>;

    fn lt(&self, left: Rc<dyn Var>, right: Rc<dyn Var>) -> Rc<dyn Var>;
    fn leq(&self, left: Rc<dyn Var>, right: Rc<dyn Var>) -> Rc<dyn Var>;
    fn geq(&self, left: Rc<dyn Var>, right: Rc<dyn Var>) -> Rc<dyn Var>;
    fn gt(&self, left: Rc<dyn Var>, right: Rc<dyn Var>) -> Rc<dyn Var>;

    fn or(&self, terms: &[Rc<dyn Var>]) -> Rc<dyn Var>;
    fn and(&self, terms: &[Rc<dyn Var>]) -> Rc<dyn Var>;

    fn assert(&self, term: Rc<dyn Var>) -> bool;
}

#[derive(Debug)]
pub enum RiddleError {
    NotAnEnvironment(String),
    NotAClass(String),
    TypeError(String),
    NotFound(String),
    RuntimeError(String),
}

pub fn execute(scp: Rc<dyn Scope>, env: Rc<dyn Env>, stmt: &Statement) -> Result<(), RiddleError> {
    match stmt {
        Statement::Expr(expr) => {
            if scp.clone().core().assert(evaluate(scp, env, expr)?) {
                Ok(())
            } else {
                Err(RiddleError::RuntimeError("Assertion failed".into()))
            }
        }
        _ => unimplemented!(),
    }
}

pub fn evaluate(scp: Rc<dyn Scope>, env: Rc<dyn Env>, expr: &Expr) -> Result<Rc<dyn Var>, RiddleError> {
    match expr {
        Expr::Bool(bool) => Ok(scp.core().new_bool(*bool)),
        Expr::Int(int) => Ok(scp.core().new_int(*int)),
        Expr::Real(num, den) => Ok(scp.core().new_real(*num, *den)),
        Expr::String(string) => Ok(scp.core().new_string(string)),
        Expr::QualifiedId { ids } => {
            let (first, rest) = ids.split_first().ok_or_else(|| RiddleError::RuntimeError("Empty identifier path".into()))?;
            let root = env.get(first).ok_or_else(|| RiddleError::NotFound(first.to_string()))?;
            rest.iter().try_fold(root, |acc, id| acc.as_env().ok_or_else(|| RiddleError::NotAnEnvironment(id.to_string()))?.get(id).ok_or_else(|| RiddleError::NotFound(format!("Member '{}' in path", id))))
        }
        Expr::Sum { terms } => {
            let evaluated_terms: Vec<Rc<dyn Var>> = terms.iter().map(|t| evaluate(scp.clone(), env.clone(), t)).collect::<Result<_, _>>()?;
            Ok(scp.core().sum(&evaluated_terms))
        }
        Expr::Opposite { term } => {
            let evaluated_term = evaluate(scp.clone(), env, term)?;
            Ok(scp.core().opposite(evaluated_term))
        }
        Expr::Mul { factors } => {
            let evaluated_factors: Vec<Rc<dyn Var>> = factors.iter().map(|f| evaluate(scp.clone(), env.clone(), f)).collect::<Result<_, _>>()?;
            Ok(scp.core().mul(&evaluated_factors))
        }
        Expr::Div { left, right } => {
            let evaluated_left = evaluate(scp.clone(), env.clone(), left)?;
            let evaluated_right = evaluate(scp.clone(), env, right)?;
            Ok(scp.core().div(evaluated_left, evaluated_right))
        }
        Expr::Function { name, args } => {
            let evaluated_args: Vec<Rc<dyn Var>> = args.iter().map(|a| evaluate(scp.clone(), env.clone(), a)).collect::<Result<_, _>>()?;
            let method = scp.get_method(name.last().unwrap(), &evaluated_args.iter().map(|arg| arg.class()).collect::<Vec<_>>()).ok_or_else(|| RiddleError::NotFound(format!("Method '{}' with specified argument types", name.join("."))))?;
            method.call(env, evaluated_args)?.ok_or_else(|| RiddleError::RuntimeError(format!("Method '{}' did not return a value", name.join("."))))
        }
        Expr::Eq { left, right } => {
            let evaluated_left = evaluate(scp.clone(), env.clone(), left)?;
            let evaluated_right = evaluate(scp.clone(), env, right)?;
            Ok(scp.core().eq(evaluated_left, evaluated_right))
        }
        Expr::Neq { left, right } => {
            let evaluated_left = evaluate(scp.clone(), env.clone(), left)?;
            let evaluated_right = evaluate(scp.clone(), env, right)?;
            Ok(scp.core().neq(evaluated_left, evaluated_right))
        }
        Expr::Lt { left, right } => {
            let evaluated_left = evaluate(scp.clone(), env.clone(), left)?;
            let evaluated_right = evaluate(scp.clone(), env, right)?;
            Ok(scp.core().lt(evaluated_left, evaluated_right))
        }
        Expr::Leq { left, right } => {
            let evaluated_left = evaluate(scp.clone(), env.clone(), left)?;
            let evaluated_right = evaluate(scp.clone(), env, right)?;
            Ok(scp.core().leq(evaluated_left, evaluated_right))
        }
        Expr::Geq { left, right } => {
            let evaluated_left = evaluate(scp.clone(), env.clone(), left)?;
            let evaluated_right = evaluate(scp.clone(), env, right)?;
            Ok(scp.core().geq(evaluated_left, evaluated_right))
        }
        Expr::Gt { left, right } => {
            let evaluated_left = evaluate(scp.clone(), env.clone(), left)?;
            let evaluated_right = evaluate(scp.clone(), env, right)?;
            Ok(scp.core().gt(evaluated_left, evaluated_right))
        }
        Expr::Or { terms } => {
            let evaluated_terms: Vec<Rc<dyn Var>> = terms.iter().map(|t| evaluate(scp.clone(), env.clone(), t)).collect::<Result<_, _>>()?;
            Ok(scp.core().or(&evaluated_terms))
        }
        Expr::And { terms } => {
            let evaluated_terms: Vec<Rc<dyn Var>> = terms.iter().map(|t| evaluate(scp.clone(), env.clone(), t)).collect::<Result<_, _>>()?;
            Ok(scp.core().and(&evaluated_terms))
        }
        Expr::NewObject { class_name, args } => {
            let (first, rest) = class_name.split_first().ok_or_else(|| RiddleError::RuntimeError("Empty class name".into()))?;
            let class = scp.get_class(first).ok_or_else(|| RiddleError::NotFound(first.to_string()))?.as_class().ok_or_else(|| RiddleError::NotAClass(first.to_string()))?;
            rest.iter().try_fold(class.clone(), |acc, id| acc.get_class(id).ok_or_else(|| RiddleError::NotFound(format!("Class '{}' in path", id)))?.as_class().ok_or_else(|| RiddleError::NotAClass(id.to_string())))?;
            let evaluated_args: Vec<Rc<dyn Var>> = args.iter().map(|a| evaluate(scp.clone(), env.clone(), a)).collect::<Result<_, _>>()?;
            let constructor = class.constructor(&evaluated_args.iter().map(|arg| arg.class()).collect::<Vec<_>>()).ok_or_else(|| RiddleError::NotFound(format!("Constructor for class '{}' with specified argument types", class_name.join("."))))?;
            constructor.call(env, evaluated_args)?.ok_or_else(|| RiddleError::RuntimeError(format!("Constructor for class '{}' did not return a value", class_name.join("."))))
        }
    }
}

pub fn arith_class(cr: Rc<dyn Core>, terms: &[Rc<dyn Var>]) -> Result<Rc<dyn Type>, RiddleError> {
    if terms.iter().all(|t| t.class().name() == "int") {
        Ok(cr.get_class("int").expect("int class not found"))
    } else if terms.iter().all(|t| t.class().name() == "real") {
        Ok(cr.get_class("real").expect("real class not found"))
    } else if terms.iter().all(|t| t.class().name() == "int" || t.class().name() == "real") {
        Ok(cr.get_class("real").expect("real class not found"))
    } else {
        Err(RiddleError::TypeError("Invalid types for arithmetic operation".into()))
    }
}

#[cfg(test)]
mod tests {
    use crate::{env::*, language::*, parse_problem};

    struct TestObject {
        class: Weak<dyn Type>,
    }

    impl Var for TestObject {
        fn class(&self) -> Rc<dyn Type> {
            self.class.upgrade().unwrap()
        }

        fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
            self
        }
    }

    struct TestCore {
        scope: Rc<CommonScope>,
        env: Rc<CommonEnv>,
    }

    impl TestCore {
        fn new() -> Rc<Self> {
            let core = Rc::new_cyclic(|core| Self {
                scope: {
                    let core: Weak<TestCore> = core.clone();
                    Rc::new(CommonScope::new(core.clone(), None))
                },
                env: Rc::new(CommonEnv::new(None)),
            });
            let core_dyn: Rc<dyn Core> = core.clone();
            core.scope.classes.borrow_mut().insert("bool".to_string(), Rc::new(BoolType::new(Rc::downgrade(&core_dyn))));
            core.scope.classes.borrow_mut().insert("int".to_string(), Rc::new(IntType::new(Rc::downgrade(&core_dyn))));
            core.scope.classes.borrow_mut().insert("real".to_string(), Rc::new(RealType::new(Rc::downgrade(&core_dyn))));
            core.scope.classes.borrow_mut().insert("string".to_string(), Rc::new(StringType::new(Rc::downgrade(&core_dyn))));
            core
        }

        fn read(&mut self, riddle: &str) {
            let mut problem = parse_problem(riddle).expect("Failed to parse problem");
            let statments = std::mem::take(&mut problem.statements);
            self.scope.add_problem(problem);
            for stmt in statments {
                execute(self.scope.clone(), self.env.clone(), &stmt).expect("Failed to execute statement");
            }
        }
    }

    impl Core for TestCore {
        fn new_bool(&self, _value: bool) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("bool").expect("bool class not found")) })
        }

        fn new_bool_var(&self) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("bool").unwrap()) })
        }

        fn new_int(&self, _value: i64) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("int").unwrap()) })
        }

        fn new_int_var(&self) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("int").unwrap()) })
        }

        fn new_real(&self, _num: i64, _den: i64) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("real").unwrap()) })
        }

        fn new_real_var(&self) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("real").unwrap()) })
        }

        fn new_string(&self, _value: &str) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("string").unwrap()) })
        }

        fn new_string_var(&self) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("string").unwrap()) })
        }

        fn sum(&self, _sum: &[Rc<dyn Var>]) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("int").unwrap()) })
        }

        fn opposite(&self, _term: Rc<dyn Var>) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("int").unwrap()) })
        }

        fn mul(&self, _mul: &[Rc<dyn Var>]) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("int").unwrap()) })
        }

        fn div(&self, _left: Rc<dyn Var>, _right: Rc<dyn Var>) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("int").unwrap()) })
        }

        fn eq(&self, _left: Rc<dyn Var>, _right: Rc<dyn Var>) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("bool").unwrap()) })
        }

        fn neq(&self, _left: Rc<dyn Var>, _right: Rc<dyn Var>) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("bool").unwrap()) })
        }

        fn lt(&self, _left: Rc<dyn Var>, _right: Rc<dyn Var>) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("bool").unwrap()) })
        }

        fn leq(&self, _left: Rc<dyn Var>, _right: Rc<dyn Var>) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("bool").unwrap()) })
        }

        fn geq(&self, _left: Rc<dyn Var>, _right: Rc<dyn Var>) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("bool").unwrap()) })
        }

        fn gt(&self, _left: Rc<dyn Var>, _right: Rc<dyn Var>) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("bool").unwrap()) })
        }

        fn or(&self, _terms: &[Rc<dyn Var>]) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("bool").unwrap()) })
        }

        fn and(&self, _terms: &[Rc<dyn Var>]) -> Rc<dyn Var> {
            Rc::new(TestObject { class: Rc::downgrade(&self.get_class("bool").unwrap()) })
        }

        fn assert(&self, _term: Rc<dyn Var>) -> bool {
            true
        }
    }

    impl Scope for TestCore {
        fn core(self: Rc<Self>) -> Rc<dyn Core> {
            self
        }

        fn parent(&self) -> Option<Rc<dyn Scope>> {
            None
        }

        fn get_field(&self, _name: &str) -> Option<Rc<Field>> {
            None
        }

        fn get_method(&self, name: &str, classes: &[Rc<dyn Type>]) -> Option<Rc<Method>> {
            self.scope.get_method(name, classes)
        }

        fn get_class(&self, name: &str) -> Option<Rc<dyn Type>> {
            self.scope.get_class(name)
        }

        fn get_enum(&self, name: &str) -> Option<Rc<EnumDef>> {
            self.scope.get_enum(name)
        }

        fn get_predicate(&self, name: &str) -> Option<Rc<PredicateDef>> {
            self.scope.get_predicate(name)
        }
    }

    impl Env for TestCore {
        fn parent(&self) -> Option<Rc<dyn Env>> {
            None
        }

        fn get(&self, _name: &str) -> Option<Rc<dyn Var>> {
            None
        }

        fn set(&self, _name: String, _value: Rc<dyn Var>) {}
    }

    #[test]
    fn create_core() {
        let core = TestCore::new();
        assert!(core.get_class("bool").is_some());
        assert!(core.get_class("int").is_some());
        assert!(core.get_class("real").is_some());
        assert!(core.get_class("string").is_some());
    }
}
