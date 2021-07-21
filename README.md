# Spock
Spock is a project aimed at making visual projects with Vulkan and C++ much easier to manage. 
It breaks up the components of the Vulkan API into self contained classes which should make constructing/reconstructing and resizing pipelines and views much easier. 

GLM is the math library of choice for this project because it is simple, lightweight, and very easy to use. Why reinvent the wheel when you should be focusing on building the cart?

This is the second iteration of project Spock. The first iteration was a sloppy attempt at a graphics framework which only really worked for the demonstration. Using what was learned from the first iteration, this iteration will attempt to produce nicer code and a more flexible framework. The first iteration was not well planned, so more time will be spent in planning and design for this iteration. 

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
>> |--: Project Scope and Requirements Planning
   |
   |--: ShaderPack planning and assessment
   |--+ ShaderPack initial implementation
   |
   |--: ColdStorage planning and assessment
   |--+ ColdStorage initial implementation
   |
   |--> ColdStorage and ShaderPack testing
   |--+ ColdStorage and ShaderPack bugfix/refactor
   |
  @|====@ ColdStorage and ShaderPack complete
   |
   |--: Spock planning and assessment
   |--+ Spock initial implementation
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
