use crate::scope::{Predicate, Type};
use std::{
    any::Any,
    cell::RefCell,
    collections::HashMap,
    rc::{Rc, Weak},
};

pub trait Var {
    fn class(&self) -> Rc<dyn Type>;
    fn as_any(self: Rc<Self>) -> Rc<dyn Any>;
    fn as_env(&self) -> Option<&dyn Env> {
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

impl Var for Atom {
    fn class(&self) -> Rc<dyn Type> {
        self.predicate()
    }

    fn as_any(self: Rc<Self>) -> Rc<dyn Any> {
        self
    }

    fn as_env(&self) -> Option<&dyn Env> {
        Some(&self.env)
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
