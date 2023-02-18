```mermaid
---
title: Class Diagram
---
classDiagram
    scope <|-- core
    env <|-- core
    scope <|-- conjunction
    scope <|-- constructor
    scope <|-- method
    item <|-- complex_item
    type <|-- complex_type
    type <|-- predicate

    class scope {
        +get_type(string name)
        +get_field(string name)
        +get_method(string name, vector<type> args)
        +get_predicate(string name)
        -map<string, field> fields
    }

    class env {
        +get(string name)
        -env parent
        -map<string, expr> items
    }

    class conjunction {
        +exectute()
        -env ctx
        -rational cost
        -vector<statement> body
    }

    class constructor {
        +call(expr self, vector<expr> args)
        -vector<field> args
        -vector<statement> body
    }

    class method {
        +call(expr self, vector<expr> args)
        -string name
        -vector<field> args
        -vector<statement> body
    }

    class item {
        +get_type()
        -type type
    }

    class complex_item {
        -map<string, expr> items
    }

    class type {
        -string name
    }

    class complex_type {
        +get_parents()
        +get_type(string name)
        +get_method(string name, vector<type> args)
        +get_predicate(string name)
        +get_instances()
        -vector<complex_type> parents
        -map<string, type> types
        -map<string, method> methods
        -map<string, predicate> predicates
        -vector<expr> instances
    }

    class predicate {
        +get_parents()
        +get_instances()
        +call(expr atm)
        -vector<predicate> parents
        -vector<field> args
        -vector<statement> body
        -vector<expr> instances
    }
```