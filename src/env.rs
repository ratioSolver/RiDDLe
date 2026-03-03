use crate::language::{ClassDef, EnumDef, Expr, MethodDef, PredicateDef, ProblemDef, Statement};
use std::{
    any::Any,
    cell::RefCell,
    collections::HashMap,
    rc::{Rc, Weak},
};

pub trait Class {
    fn name(&self) -> &str;
    fn as_any(self: Rc<Self>) -> Rc<dyn Any>;
    fn new_instance(self: Rc<Self>) -> Rc<dyn Object>;
}

pub struct BoolClass {
    core: Weak<dyn Core>,
}

impl BoolClass {
    pub fn new(core: Rc<dyn Core>) -> Self {
        Self { core: Rc::downgrade(&core) }
    }
}

impl Class for BoolClass {
    fn name(&self) -> &str {
        "bool"
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn new_instance(self: Rc<Self>) -> Rc<dyn Object> {
        self.core.upgrade().unwrap().new_bool_var()
    }
}

pub struct IntClass {
    core: Weak<dyn Core>,
}

impl IntClass {
    pub fn new(core: Rc<dyn Core>) -> Self {
        Self { core: Rc::downgrade(&core) }
    }
}

impl Class for IntClass {
    fn name(&self) -> &str {
        "int"
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn new_instance(self: Rc<Self>) -> Rc<dyn Object> {
        self.core.upgrade().unwrap().new_int_var()
    }
}

pub struct RealClass {
    core: Weak<dyn Core>,
}

impl RealClass {
    pub fn new(core: Rc<dyn Core>) -> Self {
        Self { core: Rc::downgrade(&core) }
    }
}

impl Class for RealClass {
    fn name(&self) -> &str {
        "real"
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn new_instance(self: Rc<Self>) -> Rc<dyn Object> {
        self.core.upgrade().unwrap().new_real_var()
    }
}

pub struct StringClass {
    core: Weak<dyn Core>,
}

impl StringClass {
    pub fn new(core: Rc<dyn Core>) -> Self {
        Self { core: Rc::downgrade(&core) }
    }
}

impl Class for StringClass {
    fn name(&self) -> &str {
        "string"
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn new_instance(self: Rc<Self>) -> Rc<dyn Object> {
        self.core.upgrade().unwrap().new_string_var()
    }
}

pub struct Field {
    name: String,
    field_type: Vec<String>,
    default: Option<Expr>,
}

pub trait Object {
    fn class(&self) -> Rc<dyn Class>;
    fn as_any(self: Rc<Self>) -> Rc<dyn Any>;
    fn as_env(&self) -> Option<&dyn Env> {
        None
    }
}

pub trait Scope {
    fn core(self: Rc<Self>) -> Rc<dyn Core>;
    fn parent(&self) -> Option<Rc<dyn Scope>>;

    fn get_field(&self, name: &str) -> Option<Rc<Field>>;
    fn get_method(&self, name: &str) -> Option<Rc<Method>>;
    fn get_class(&self, name: &str) -> Option<Rc<dyn Class>>;
    fn get_enum(&self, name: &str) -> Option<Rc<EnumDef>>;
    fn get_predicate(&self, name: &str) -> Option<Rc<PredicateDef>>;
}

pub trait Env {
    fn parent(&self) -> Option<Rc<dyn Env>>;
    fn get(&self, name: &str) -> Option<Rc<dyn Object>>;
}

pub trait EnvExt {
    fn get_as<T: Object + 'static>(&self, name: &str) -> Option<Rc<T>>;
}

impl<E: Env + ?Sized> EnvExt for E {
    fn get_as<T: Object + 'static>(&self, name: &str) -> Option<Rc<T>> {
        self.get(name)?.as_any().downcast::<T>().ok()
    }
}

pub struct CommonEnv {
    parent: Option<Rc<dyn Env>>,
    variables: RefCell<HashMap<String, Rc<dyn Object>>>,
}

pub struct CommonScope {
    core: Weak<dyn Core>,
    parent: Option<Rc<dyn Scope>>,
    fields: RefCell<HashMap<String, Rc<Field>>>,
    methods: RefCell<HashMap<String, Vec<Rc<Method>>>>,
    classes: RefCell<HashMap<String, Rc<dyn Class>>>,
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

    fn get(&self, name: &str) -> Option<Rc<dyn Object>> {
        self.variables.borrow().get(name).cloned().or_else(|| self.parent.as_ref()?.get(name))
    }
}

impl CommonScope {
    pub fn new(core: Rc<dyn Core>, parent: Option<Rc<dyn Scope>>) -> Self {
        Self {
            core: Rc::downgrade(&core),
            parent,
            fields: RefCell::new(HashMap::new()),
            methods: RefCell::new(HashMap::new()),
            classes: RefCell::new(HashMap::new()),
            enums: RefCell::new(HashMap::new()),
            predicates: RefCell::new(HashMap::new()),
        }
    }

    pub fn from_class(core: Rc<dyn Core>, parent: Option<Rc<dyn Scope>>, class: ClassDef) -> Rc<Self> {
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

    pub fn from_method(core: Rc<dyn Core>, parent: Option<Rc<dyn Scope>>, method: MethodDef) -> Self {
        let scope = Self::new(core, parent);
        for (arg_type, arg_name) in method.args {
            scope.fields.borrow_mut().insert(arg_name.clone(), Rc::new(Field { name: arg_name, field_type: arg_type, default: None }));
        }
        scope
    }

    pub fn add_problem(&self, problem: ProblemDef) {
        for method in problem.methods {
            self.methods.borrow_mut().entry(method.name.clone()).or_default().push(Rc::new(Method::new(self.core.upgrade().unwrap(), Some(self.core.upgrade().unwrap()), method)));
        }
        for predicate in problem.predicates {
            self.predicates.borrow_mut().insert(predicate.name.clone(), Rc::new(predicate));
        }
        for class in problem.classes {
            self.classes.borrow_mut().insert(class.name.clone(), Rc::new(CompositeClass::new(self.core.upgrade().unwrap(), Some(self.core.upgrade().unwrap()), class)));
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

    fn get_method(&self, name: &str) -> Option<Rc<Method>> {
        self.methods.borrow().get(name).and_then(|ms| ms.first().cloned()).or_else(|| self.parent.as_ref()?.get_method(name))
    }

    fn get_class(&self, name: &str) -> Option<Rc<dyn Class>> {
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
    scope: CommonScope,
    name: String,
    return_type: Option<Vec<String>>,
    args: Vec<(Vec<String>, String)>,
    statements: Vec<Statement>,
}

impl Method {
    pub fn new(core: Rc<dyn Core>, parent: Option<Rc<dyn Scope>>, mut method: MethodDef) -> Self {
        Self {
            core: Rc::downgrade(&core),
            name: std::mem::take(&mut method.name),
            return_type: std::mem::take(&mut method.return_type),
            args: std::mem::take(&mut method.args),
            statements: std::mem::take(&mut method.statements),
            scope: CommonScope::from_method(core, parent, method),
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

    fn get_method(&self, name: &str) -> Option<Rc<Method>> {
        self.scope.get_method(name)
    }

    fn get_class(&self, name: &str) -> Option<Rc<dyn Class>> {
        self.scope.get_class(name)
    }

    fn get_enum(&self, name: &str) -> Option<Rc<EnumDef>> {
        self.scope.get_enum(name)
    }

    fn get_predicate(&self, name: &str) -> Option<Rc<PredicateDef>> {
        self.scope.get_predicate(name)
    }
}

pub struct CompositeClass {
    core: Weak<dyn Core>,
    scope: CommonScope,
    name: String,
    instances: RefCell<Vec<Rc<CompositeObject>>>,
}

impl CompositeClass {
    pub fn new(core: Rc<dyn Core>, parent: Option<Rc<dyn Scope>>, class: ClassDef) -> Self {
        Self {
            core: Rc::downgrade(&core),
            scope: CommonScope::new(core, parent),
            name: class.name.clone(),
            instances: RefCell::new(Vec::new()),
        }
    }
}

impl Class for CompositeClass {
    fn name(&self) -> &str {
        &self.name
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn new_instance(self: Rc<Self>) -> Rc<dyn Object> {
        let instance = Rc::new(CompositeObject::new(self.clone(), None));
        self.instances.borrow_mut().push(instance.clone());
        instance
    }
}

impl Scope for CompositeClass {
    fn core(self: Rc<Self>) -> Rc<dyn Core> {
        self.core.upgrade().unwrap()
    }

    fn parent(&self) -> Option<Rc<dyn Scope>> {
        self.scope.parent.clone()
    }

    fn get_field(&self, name: &str) -> Option<Rc<Field>> {
        self.scope.get_field(name)
    }

    fn get_method(&self, name: &str) -> Option<Rc<Method>> {
        self.scope.get_method(name)
    }

    fn get_class(&self, name: &str) -> Option<Rc<dyn Class>> {
        self.scope.get_class(name)
    }

    fn get_enum(&self, name: &str) -> Option<Rc<EnumDef>> {
        self.scope.get_enum(name)
    }

    fn get_predicate(&self, name: &str) -> Option<Rc<PredicateDef>> {
        self.scope.get_predicate(name)
    }
}

pub struct CompositeObject {
    class: Rc<dyn Class>,
    env: CommonEnv,
}

impl CompositeObject {
    pub fn new(class: Rc<dyn Class>, parent_env: Option<Rc<dyn Env>>) -> Self {
        Self { class, env: CommonEnv::new(parent_env) }
    }
}

impl Object for CompositeObject {
    fn class(&self) -> Rc<dyn Class> {
        self.class.clone()
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn as_env(&self) -> Option<&dyn Env> {
        Some(&self.env)
    }
}

impl Env for CompositeObject {
    fn parent(&self) -> Option<Rc<dyn Env>> {
        self.env.parent.clone()
    }

    fn get(&self, name: &str) -> Option<Rc<dyn Object>> {
        self.env.get(name)
    }
}

pub trait Core: Scope + Env {
    fn new_bool(&self, value: bool) -> Rc<dyn Object>;
    fn new_bool_var(&self) -> Rc<dyn Object>;
    fn new_int(&self, value: i64) -> Rc<dyn Object>;
    fn new_int_var(&self) -> Rc<dyn Object>;
    fn new_real(&self, num: i64, den: i64) -> Rc<dyn Object>;
    fn new_real_var(&self) -> Rc<dyn Object>;
    fn new_string(&self, value: String) -> Rc<dyn Object>;
    fn new_string_var(&self) -> Rc<dyn Object>;
}
