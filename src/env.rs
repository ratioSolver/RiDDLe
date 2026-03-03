use std::{
    any::Any,
    cell::RefCell,
    collections::HashMap,
    rc::{Rc, Weak},
};

use crate::language::{ClassDef, EnumDef, Expr, FieldDef, MethodDef, PredicateDef, ProblemDef};

pub trait Class {
    fn name(&self) -> &str;
    fn as_any(self: Rc<Self>) -> Rc<dyn Any>;
    fn new_instance(self: Rc<Self>) -> Rc<dyn Object>;
}

pub struct BoolClass {
    core: Weak<dyn Core>,
}

pub struct IntClass {
    core: Weak<dyn Core>,
}

pub struct RealClass {
    core: Weak<dyn Core>,
}

pub struct StringClass {
    core: Weak<dyn Core>,
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
    fn get_method(&self, name: &str) -> Option<Rc<MethodDef>>;
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
    methods: RefCell<HashMap<String, Vec<Rc<MethodDef>>>>,
    classes: RefCell<HashMap<String, Rc<dyn Class>>>,
    enums: RefCell<HashMap<String, Rc<EnumDef>>>,
    predicates: RefCell<HashMap<String, Rc<PredicateDef>>>,
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

pub struct CompositeClass {
    core: Weak<dyn Core>,
    name: String,
    scope: Rc<CommonScope>,
}

impl CompositeClass {
    pub fn new(core: Rc<dyn Core>, class: ClassDef) -> Self {
        Self {
            core: Rc::downgrade(&core),
            name: class.name.clone(),
            scope: Rc::new(CommonScope::new(core, None)),
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
        unimplemented!()
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

    fn get_method(&self, name: &str) -> Option<Rc<MethodDef>> {
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

    pub fn from_class(core: Rc<dyn Core>, parent: Option<Rc<dyn Scope>>, class: ClassDef) -> Self {
        let scope = Self::new(core, parent);
        for (field_type, fields) in class.fields {
            for (name, default) in fields {
                scope.fields.borrow_mut().insert(name.clone(), Rc::new(Field { name, field_type: field_type.clone(), default }));
            }
        }
        for method in class.methods {
            scope.methods.borrow_mut().entry(method.name.clone()).or_default().push(Rc::new(method));
        }
        for predicate in class.predicates {
            scope.predicates.borrow_mut().insert(predicate.name.clone(), Rc::new(predicate));
        }
        scope
    }

    pub fn add_problem(&self, problem: ProblemDef) {
        for method in problem.methods {
            self.methods.borrow_mut().entry(method.name.clone()).or_default().push(Rc::new(method));
        }
        for predicate in problem.predicates {
            self.predicates.borrow_mut().insert(predicate.name.clone(), Rc::new(predicate));
        }
        for class in problem.classes {
            self.classes.borrow_mut().insert(class.name.clone(), Rc::new(CompositeClass::new(self.core.upgrade().unwrap(), class)));
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

    fn get_method(&self, name: &str) -> Option<Rc<MethodDef>> {
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
