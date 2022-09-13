# The RiDDLe Language

The Rational Domain Definition Language (RiDDLe) takes inspiration from the [DDL.1](https://www.researchgate.net/publication/228818262_DDL_1_A_formal_description_of_a_constraint_representation_language_for_physical_domains) language and extends it introducing a pure object-oriented approach to the definition of timeline-based domains and problem definitions, allowing an higher decomposition of the domain model and an increase of modularity with a consequent reduction of the the overall complexity at design phase.

This repository contains a C++ parser of the RiDDLe language, providing the data structures that facilitate the integration of the parser with a solver. By inheriting from the [parser](https://github.com/ratioSolver/RiDDLe/blob/main/include/riddle_parser.h) class, in particular, it is possible to easily develop a parser that returns the data structures customized to the needs of the reasoner.

A reacher description of the RiDDLe language can be found [here](https://github.com/ratioSolver/RiDDLe/wiki).
