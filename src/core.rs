use crate::{
    env::{CommonEnv, Env, Var},
    language::{Disjunction, EnumDef, execute},
    parse_problem,
    scope::{BoolType, CommonScope, Field, IntType, Method, Predicate, RealType, Scope, StringType, Type},
};
use std::rc::{Rc, Weak};

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
    fn new_disjunction(&self, disjunction: Disjunction);
}

pub struct CommonCore {
    scope: Rc<CommonScope>,
    env: Rc<CommonEnv>,
}

impl CommonCore {
    pub fn new(core: Weak<dyn Core>) -> Rc<Self> {
        let c_core = Rc::new(CommonCore { scope: Rc::new(CommonScope::new(core.clone(), None)), env: Rc::new(CommonEnv::new(None)) });
        c_core.scope.classes.borrow_mut().insert("bool".to_string(), Rc::new(BoolType::new(core.clone())));
        c_core.scope.classes.borrow_mut().insert("int".to_string(), Rc::new(IntType::new(core.clone())));
        c_core.scope.classes.borrow_mut().insert("real".to_string(), Rc::new(RealType::new(core.clone())));
        c_core.scope.classes.borrow_mut().insert("string".to_string(), Rc::new(StringType::new(core.clone())));
        c_core
    }

    pub fn bool_type(&self) -> Rc<BoolType> {
        self.scope.get_class("bool").expect("Core should have bool type").as_any().downcast::<BoolType>().expect("Core bool type should be BoolType")
    }

    pub fn int_type(&self) -> Rc<IntType> {
        self.scope.get_class("int").expect("Core should have int type").as_any().downcast::<IntType>().expect("Core int type should be IntType")
    }

    pub fn real_type(&self) -> Rc<RealType> {
        self.scope.get_class("real").expect("Core should have real type").as_any().downcast::<RealType>().expect("Core real type should be RealType")
    }

    pub fn string_type(&self) -> Rc<StringType> {
        self.scope.get_class("string").expect("Core should have string type").as_any().downcast::<StringType>().expect("Core string type should be StringType")
    }

    pub fn read(&self, riddle: &str) {
        let mut problem = parse_problem(riddle).expect("Failed to parse problem");
        let statments = std::mem::take(&mut problem.statements);
        self.scope.add_problem(problem);
        for stmt in statments {
            execute(self.scope.clone(), self.env.clone(), &stmt).expect("Failed to execute statement");
        }
    }
}

impl Scope for CommonCore {
    fn core(self: Rc<Self>) -> Rc<dyn Core> {
        self.scope.clone().core()
    }

    fn parent(&self) -> Option<Rc<dyn Scope>> {
        None
    }

    fn get_field(&self, _name: &str) -> Option<Rc<Field>> {
        self.scope.get_field(_name)
    }

    fn get_method(&self, _name: &str, _classes: &[Rc<dyn Type>]) -> Option<Rc<Method>> {
        self.scope.get_method(_name, _classes)
    }

    fn get_class(&self, name: &str) -> Option<Rc<dyn Type>> {
        self.scope.get_class(name)
    }

    fn get_enum(&self, _name: &str) -> Option<Rc<EnumDef>> {
        self.scope.get_enum(_name)
    }

    fn get_predicate(&self, _name: &str) -> Option<Rc<Predicate>> {
        self.scope.get_predicate(_name)
    }
}

impl Env for CommonCore {
    fn parent(&self) -> Option<Rc<dyn Env>> {
        None
    }

    fn get(&self, name: &str) -> Option<Rc<dyn Var>> {
        self.env.get(name)
    }

    fn set(&self, name: String, value: Rc<dyn Var>) {
        self.env.set(name, value);
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::any::Any;

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
        core: Rc<CommonCore>,
    }

    impl TestCore {
        fn new() -> Rc<Self> {
            Rc::new_cyclic(|core| Self {
                core: {
                    let core: Weak<TestCore> = core.clone();
                    CommonCore::new(core)
                },
            })
        }

        fn read(&self, riddle: &str) {
            self.core.read(riddle);
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

        fn new_disjunction(&self, _disjunction: Disjunction) {}
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
            self.core.get_method(name, classes)
        }

        fn get_class(&self, name: &str) -> Option<Rc<dyn Type>> {
            self.core.get_class(name)
        }

        fn get_enum(&self, name: &str) -> Option<Rc<EnumDef>> {
            self.core.get_enum(name)
        }

        fn get_predicate(&self, name: &str) -> Option<Rc<Predicate>> {
            self.core.get_predicate(name)
        }
    }

    impl Env for TestCore {
        fn parent(&self) -> Option<Rc<dyn Env>> {
            None
        }

        fn get(&self, name: &str) -> Option<Rc<dyn Var>> {
            self.core.get(name)
        }

        fn set(&self, name: String, value: Rc<dyn Var>) {
            self.core.set(name, value);
        }
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
