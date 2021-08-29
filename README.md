# Spock
Spock is a project aimed at making visual projects with Vulkan and C++ much easier to manage. 
It breaks up the components of the Vulkan API into self contained classes which should make constructing/reconstructing and resizing pipelines and views much easier. 

GLM is the math library of choice for this project because it is simple, lightweight, and very easy to use. Why reinvent the wheel when you should be focusing on building the cart?

This is the second iteration of project Spock. The first iteration was a sloppy attempt at a graphics framework which only really worked for the demonstration. Using what was learned from the first iteration, this iteration will attempt to produce nicer code and a more flexible framework. The first iteration was not well planned, so more time will be spent in planning and design for this iteration. 

---
## Note to potential recruiters
---
If you're here from my resume, check out the code in the [old-code](https://github.com/zachmakesgames/Spock/tree/old-code) branch.
The old code should work, but its limited which is why I'm working on redesigning the framework.

<br/>
<br/>

---
## Timeline
---
This is a personal project, so there are no hard deliverable dates for the timeline. I work a day job and theres only so much I can do in a day. Rather than dates, this timeline represents milestones for the project. This is a large project with multiple components that rely on each other, so some components must be completed before others. 

```
Legend:
   @|====@ Milestone
    |--: Planning Action
    |--+ Development Action
    |--> Testing Action
 >> |  Current project state
```

```
   @|====@ Iteration 2 Begins  
    |
    |--: Project Scope and Requirements Planning
    |
    |--: Spock planning and assessment
 >> |--+ Spock initial implementation
    |
    |--+ Begin work on demo to facilitate Spock testing
    |
    |--> Spock initial testing
    |--+ Spock bugfix/refactor
    |
   @|====@ Spock complete
    |
    |--: Tech demo planning and assessment
    |
    |--+ Tech demo initial implementation
    |
    |--> Tech demo testing and assessment
    |--+ Tech demo bugfix/refactor
    |
   @|====@ Tech demo complete
   @|====@ Project complete
```
