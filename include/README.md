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
```
