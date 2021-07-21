# Code Conventions

The following code conventions should be followed to ensure proper readability and uniformity between projects in this solution. The convetions are designed so that the purpose and function of code can be easily determined by name alone.

All naming conventions are based on camel case. The convention for each type of name describe how camel case is modified for that specific type.


## Variable Names
 ---
Variables should be named in plain text with minimal abbreviation. When abbreviating for a common word, the same abbreviation should be used consistently throughout all code. Variable names should be as descriptive as possible to avoid confusion. 

Class member variables should always start with a lower case abbreviation of the class they are a member of. The first word of non member variables should always be lower case.

Global defines should be in all upper case letters. In this case an underscore is permitted between words for better readability.

Local variables should use lower case snake case to differentiate them from globals and member variables. 

### Examples
---
* Members of MyClassName: mcnMyVariableName
* Non member: myVariableName
* One word defines: DEBUG, VERSION, VULKAN, etc
* Multi-word defines: VULKAN_VERSION, VERBOSITY_LOW, VERBOSITY_HIGH, etc
* Local variables: my_local_variable, object_x, object_position, etc


## Class Names
---
Class names should always be in camel case with the first letter of each word being upper case. Class names should be as descriptive as possible

### Examples
---
* MyClassName
* Pipeline
* DescriptorSet