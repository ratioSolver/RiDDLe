use std::{
    any::Any,
    rc::{Rc, Weak},
};

use crate::language::{EnumDef, Field, MethodDef, PredicateDef};

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

pub trait Object {
    fn class(&self) -> Rc<dyn Class>;
    fn as_any(self: Rc<Self>) -> Rc<dyn Any>;
    fn as_env(&self) -> Option<&dyn Env> {
        None
    }
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

pub trait Scope {
    fn core(self: Rc<Self>) -> Rc<dyn Core>;
    fn parent(&self) -> Option<Rc<dyn Scope>>;

    fn get_field(&self, name: &str) -> Option<Field>;
    fn get_method(&self, name: &str) -> Option<MethodDef>;
    fn get_class(&self, name: &str) -> Option<Rc<dyn Class>>;
    fn get_enum(&self, name: &str) -> Option<EnumDef>;
    fn get_predicate(&self, name: &str) -> Option<PredicateDef>;
}

pub trait Core: Scope {
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
