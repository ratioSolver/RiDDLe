use crate::scope::{BoolType, Predicate, Scope, Type};
use std::{
    any::Any,
    cell::RefCell,
    collections::HashMap,
    rc::{Rc, Weak},
};

pub trait Var {
    fn var_type(&self) -> Rc<dyn Type>;
    fn as_any(self: Rc<Self>) -> Rc<dyn Any>;
    fn as_env(self: Rc<Self>) -> Option<Rc<dyn Env>> {
        None
    }
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

pub struct Atom {
    predicate: Weak<Predicate>,
    fact: bool,
    env: CommonEnv,
}

impl Atom {
    pub fn new(predicate: Rc<Predicate>, fact: bool, args: HashMap<String, Rc<dyn Var>>) -> Self {
        let env = args.get("tau").and_then(|tau| tau.clone().as_env()).unwrap_or_else(|| predicate.clone().core().clone());
        Self { predicate: Rc::downgrade(&predicate), fact, env: CommonEnv::new(Some(env)) }
    }

    pub fn predicate(&self) -> Rc<Predicate> {
        self.predicate.upgrade().unwrap()
    }

    pub fn is_fact(&self) -> bool {
        self.fact
    }
}

impl Var for Atom {
    fn var_type(&self) -> Rc<dyn Type> {
        self.predicate()
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn as_env(self: Rc<Self>) -> Option<Rc<dyn Env>> {
        Some(self.clone())
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
    fn var_type(&self) -> Rc<dyn Type> {
        self.class.upgrade().unwrap()
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn as_env(self: Rc<Self>) -> Option<Rc<dyn Env>> {
        Some(self.clone())
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

pub struct EqVar {
    pub var_type: Weak<BoolType>,
    pub left: Rc<dyn Var>,
    pub right: Rc<dyn Var>,
}

impl EqVar {
    pub fn new(var_type: Rc<BoolType>, left: Rc<dyn Var>, right: Rc<dyn Var>) -> Self {
        Self { var_type: Rc::downgrade(&var_type), left, right }
    }
}

impl Var for EqVar {
    fn var_type(&self) -> Rc<dyn Type> {
        self.var_type.upgrade().unwrap()
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }
}

pub struct NeqVar {
    pub var_type: Weak<BoolType>,
    pub left: Rc<dyn Var>,
    pub right: Rc<dyn Var>,
}

impl NeqVar {
    pub fn new(var_type: Rc<BoolType>, left: Rc<dyn Var>, right: Rc<dyn Var>) -> Self {
        Self { var_type: Rc::downgrade(&var_type), left, right }
    }
}

impl Var for NeqVar {
    fn var_type(&self) -> Rc<dyn Type> {
        self.var_type.upgrade().unwrap()
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }
}

pub struct LtVar {
    pub var_type: Weak<BoolType>,
    pub left: Rc<dyn Var>,
    pub right: Rc<dyn Var>,
}

impl LtVar {
    pub fn new(var_type: Rc<BoolType>, left: Rc<dyn Var>, right: Rc<dyn Var>) -> Self {
        Self { var_type: Rc::downgrade(&var_type), left, right }
    }
}

impl Var for LtVar {
    fn var_type(&self) -> Rc<dyn Type> {
        self.var_type.upgrade().unwrap()
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }
}

pub struct LeqVar {
    pub var_type: Weak<BoolType>,
    pub left: Rc<dyn Var>,
    pub right: Rc<dyn Var>,
}

impl LeqVar {
    pub fn new(var_type: Rc<BoolType>, left: Rc<dyn Var>, right: Rc<dyn Var>) -> Self {
        Self { var_type: Rc::downgrade(&var_type), left, right }
    }
}

impl Var for LeqVar {
    fn var_type(&self) -> Rc<dyn Type> {
        self.var_type.upgrade().unwrap()
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }
}
