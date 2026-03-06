use crate::{
    core::Core,
    env::{Atom, CommonEnv, Env, Object, Var},
    language::{ClassDef, ConstructorDef, EnumDef, Expr, MethodDef, PredicateDef, ProblemDef, RiddleError, Statement, execute},
};
use std::{
    any::Any,
    cell::RefCell,
    collections::HashMap,
    rc::{Rc, Weak},
};

pub trait Type {
    fn name(&self) -> &str;
    fn full_name(&self) -> String {
        self.name().to_string()
    }
    fn as_any(self: Rc<Self>) -> Rc<dyn Any>;
    fn as_class(self: Rc<Self>) -> Option<Rc<dyn Class>> {
        None
    }
    fn as_predicate(self: Rc<Self>) -> Option<Rc<Predicate>> {
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

pub trait Scope {
    fn as_class(self: Rc<Self>) -> Option<Rc<dyn Class>> {
        None
    }
    fn core(self: Rc<Self>) -> Rc<dyn Core>;
    fn scope(&self) -> Option<Rc<dyn Scope>>;

    fn get_field(&self, name: &str) -> Option<Rc<Field>>;
    fn get_method(&self, name: &str, types: &[Rc<dyn Type>]) -> Option<Rc<Method>>;
    fn get_class(&self, name: &str) -> Option<Rc<dyn Type>>;
    fn get_enum(&self, name: &str) -> Option<Rc<EnumDef>>;
    fn get_predicate(&self, name: &str) -> Option<Rc<Predicate>>;
}

pub struct CommonScope {
    core: Weak<dyn Core>,
    scope: Option<Rc<dyn Scope>>,
    fields: RefCell<HashMap<String, Rc<Field>>>,
    methods: RefCell<HashMap<String, Vec<Rc<Method>>>>,
    pub(crate) classes: RefCell<HashMap<String, Rc<dyn Type>>>,
    enums: RefCell<HashMap<String, Rc<EnumDef>>>,
    predicates: RefCell<HashMap<String, Rc<Predicate>>>,
}

impl CommonScope {
    pub fn new(core: Weak<dyn Core>, parent: Option<Rc<dyn Scope>>) -> Self {
        Self {
            core,
            scope: parent,
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
        for method_def in class.methods {
            scope.methods.borrow_mut().entry(method_def.name.clone()).or_default().push(Rc::new(Method::new(core.clone(), Some(scope.clone()), method_def)));
        }
        for predicate_def in class.predicates {
            scope.predicates.borrow_mut().insert(predicate_def.name.clone(), Rc::new(Predicate::new(core.clone(), Some(scope.clone()), predicate_def)));
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
        for method_def in problem.methods {
            self.methods.borrow_mut().entry(method_def.name.clone()).or_default().push(Rc::new(Method::new(self.core.clone(), Some(self.core.upgrade().unwrap()), method_def)));
        }
        for predicate_def in problem.predicates {
            self.predicates.borrow_mut().insert(predicate_def.name.clone(), Rc::new(Predicate::new(self.core.clone(), Some(self.core.upgrade().unwrap()), predicate_def)));
        }
        for class_def in problem.classes {
            self.classes.borrow_mut().insert(class_def.name.clone(), Rc::new(CommonClass::new(self.core.clone(), Some(self.core.upgrade().unwrap()), class_def)));
        }
        for enum_def in problem.enums {
            self.enums.borrow_mut().insert(enum_def.name.clone(), Rc::new(enum_def));
        }
    }
}

impl Scope for CommonScope {
    fn core(self: Rc<Self>) -> Rc<dyn Core> {
        self.core.upgrade().unwrap()
    }

    fn scope(&self) -> Option<Rc<dyn Scope>> {
        self.scope.clone()
    }

    fn get_field(&self, name: &str) -> Option<Rc<Field>> {
        self.fields.borrow().get(name).cloned().or_else(|| self.scope.as_ref()?.get_field(name))
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
            .or_else(|| self.scope.as_ref()?.get_method(name, types))
    }

    fn get_class(&self, name: &str) -> Option<Rc<dyn Type>> {
        self.classes.borrow().get(name).cloned().or_else(|| self.scope.as_ref()?.get_class(name))
    }

    fn get_enum(&self, name: &str) -> Option<Rc<EnumDef>> {
        self.enums.borrow().get(name).cloned().or_else(|| self.scope.as_ref()?.get_enum(name))
    }

    fn get_predicate(&self, name: &str) -> Option<Rc<Predicate>> {
        self.predicates.borrow().get(name).cloned().or_else(|| self.scope.as_ref()?.get_predicate(name))
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

    fn scope(&self) -> Option<Rc<dyn Scope>> {
        self.scope.scope.clone()
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

    fn get_predicate(&self, name: &str) -> Option<Rc<Predicate>> {
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
        let class = self.scope.scope.as_ref().expect("Constructor scope should have a parent").clone().as_class().expect("Constructor scope parent should be a class");
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

    fn scope(&self) -> Option<Rc<dyn Scope>> {
        self.scope.scope.clone()
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

    fn get_predicate(&self, name: &str) -> Option<Rc<Predicate>> {
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

impl Type for Predicate {
    fn name(&self) -> &str {
        &self.name
    }

    fn full_name(&self) -> String {
        if self.scope.scope.is_none() {
            self.name.clone()
        } else {
            let class = self.scope.scope.as_ref().unwrap().clone().as_class().unwrap();
            format!("{}.{}", class.full_name(), self.name)
        }
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn as_predicate(self: Rc<Self>) -> Option<Rc<Predicate>> {
        Some(self)
    }

    fn new_instance(self: Rc<Self>) -> Rc<dyn Var> {
        panic!("Cannot create instance of a predicate");
    }
}

impl Scope for Predicate {
    fn core(self: Rc<Self>) -> Rc<dyn Core> {
        self.core.upgrade().unwrap()
    }

    fn scope(&self) -> Option<Rc<dyn Scope>> {
        self.scope.scope.clone()
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

    fn get_predicate(&self, name: &str) -> Option<Rc<Predicate>> {
        self.scope.get_predicate(name)
    }
}

pub trait Class: Type + Scope {
    fn parents(&self) -> &[Vec<String>];
    fn constructors(&self) -> &[Constructor];
    fn constructor(&self, args: &[Rc<dyn Type>]) -> Option<&Constructor>;
    fn instances(&self) -> Vec<Rc<Object>>;
}

pub fn is_assignable_from(target: &Rc<dyn Type>, source: &Rc<dyn Type>) -> bool {
    if Rc::ptr_eq(target, source) {
        return true;
    }
    if let Some(target_class) = target.clone().as_class()
        && let Some(source_class) = source.clone().as_class()
    {
        for parent in source_class.parents() {
            if parent.iter().map(|s| s.as_str()).eq(target_class.full_name().split('.')) {
                return true;
            }
        }
        for parent in target_class.parents() {
            if parent.iter().map(|s| s.as_str()).eq(source_class.full_name().split('.')) {
                return true;
            }
        }
    }
    false
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

    fn full_name(&self) -> String {
        if self.scope.scope.is_none() {
            self.name.clone()
        } else {
            let class = self.scope.scope.as_ref().unwrap().clone().as_class().unwrap();
            format!("{}.{}", class.full_name(), self.name)
        }
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

    fn scope(&self) -> Option<Rc<dyn Scope>> {
        self.scope.scope.clone()
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

    fn get_predicate(&self, name: &str) -> Option<Rc<Predicate>> {
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
        let mut instances = self.instances.borrow().clone();
        for parent in &self.parents {
            if let Some(parent_class) = self.core.upgrade().unwrap().get_class(&parent.join(".")) {
                if let Some(parent_class) = parent_class.as_class() {
                    instances.extend(parent_class.instances());
                }
            }
        }
        instances
    }
}

pub fn arith_class(cr: Rc<dyn Core>, terms: &[Rc<dyn Var>]) -> Result<Rc<dyn Type>, RiddleError> {
    if terms.iter().all(|t| t.class().name() == "int") {
        Ok(cr.get_class("int").expect("int class not found"))
    } else if terms.iter().all(|t| t.class().name() == "int" || t.class().name() == "real") {
        Ok(cr.get_class("real").expect("real class not found"))
    } else {
        Err(RiddleError::TypeError("Invalid types for arithmetic operation".into()))
    }
}
