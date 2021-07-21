# Project Whiteboard
---

## Vulkan wrapper notes
---
Vulkan uses Instances, Devices, and Views to build up the pipeline. These pieces must be constructed in order: Instance -> Device -> View -> Pipeline, and deconstructed in reverse order.
The Instance, Device, and View are common to all shaders (although different views may be needed in some cases as well) and will be bundled together in a class called InstanceDeviceView, or IDV.

![IDV Diagram](WhiteboardPictures/IDVDiagram.jpg)

In some capacity or another, the IDV will need to facilitate the following methods:
* Load shader
* Build IDV
* Destroy IDV
* Resize view
* Render
* Display
* Mask view
* Scisor view
* Produce pipeline
* Produce buffer

<br/>

---
## Shader descriptor sets
---
In the initial tests of Vulkan, the ColdStorage tool would parse shaders and produce a consumable ShaderPack that Spock could use to create descriptor sets dynamically based on how the shaders were actually written. This worked alright, but didn't handle the camera very well. Every shader should have at the very minimal a descriptor set for the camera, and for object transform. The object transform will be different for each object, but the camera should be the same descriptor set for every object. We want the camera descriptor set to be controlled by the pipeline which means it needs to be in the same place in every shader. 

So in all shaders moving forward, descriptor set 0 binding 0 MUST be the camera, and set 1 binding 0 MUST be the object transforms.