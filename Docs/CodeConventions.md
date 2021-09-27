# Overview
This document outlines conventions and styles that should be followed for all code in this repository. These conventions are shamelessly ripped from a Bungie Tech Blog on the conventions that they use. These
conventions will be based on Bungie's 6 razors which will help create new guidelines for this project. 

[BUNGIE C++ GUIDELINES & RAZORS](https://www.bungie.net/en/Explore/Detail/News/50666)

# 6 Razors


## 1. FAVOR UNDERSTANDABILITY AT THE EXPENSE OF TIME-TO-WRITE
Every line of code will be read many times by many people of varying backgrounds for every time an expert edits it, so prefer explicit-but-verbose to concise-but-implicit.

## 2. AVOID DISTINCTION WITHOUT DIFFERENCE
When possible without loss of generality, reduce mental tax by proscribing redundant and arbitrary alternatives.

## 3. LEVERAGE VISUAL CONSISTENCY
Use visually-distinct patterns to convey complexity and signpost hazards

## 4. AVOID MISLEADING ABSTRACTIONS
When hiding complexity, signpost characteristics that are important for the customer to understand.

## 5. FAVOR PATTERNS THAT MAKE CODE MORE ROBUST
It’s desirable to reduce the odds that a future change (or a conflicting change in another branch) introduces a non-obvious bug and to facilitate finding bugs, because we spend far more time extending and debugging than implementing.

## 6. CENTRALIZE LIFECYCLE MANAGEMENT
Distributing lifecycle management across systems with different policies makes it difficult to reason about correctness when composing systems and behaviors. Instead, leverage the shared toolbox and idioms and avoid managing your own lifecycle whenever possible.



# Conventions and Guidelines

* Use snake_case for naming convention
* Avoid abbreviation of words
* Add helpful inline comments often
* Use American English spellings
* Use post increment in general usage
* \* and \& go next to the variable name instead of the type name (int32 \*my_pointer over int32\* my_pointer)
* Use tabs instead of spaces for indentation
* Braces go on their own line, except for one line function definitions
* Use all uppercase for preprocessor directive (eg #ifdef DEBUG_BUILD)
* Do not use a space to the left of assignment operators to distinguish from comparisons (eg my_variable= 10 vs my_variable == 10)
* Leverage pointer operators (\*/\&/\->) to advertise memory indirection instead of references
* Use standardized prefixes on variables and types for quick recognition
    * c_ for class types, e_ for enums
    * m_ for member variables, k_ for constants
* Function names should have obvious implications
    * get_blank() should have a trivial cost
    * try_to_get_blank() may fail, but will do so gracefully
    * compute_blank() or query_blank() are expected to have a non-trivial cost
* No operator overloading for non-standard functionality
* Initialize variables at declaration time
* Follow const correctness principles for class interfaces
* Single return statement at the bottom of a function
* Leverage asserts to validate state
* Avoid native arrays and use our own containers
* Avoid using the Standard Template Library
