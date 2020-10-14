# Spock
Spock is a project aimed at making visual projects with Vulkan and C++ much easier to manage. 
It wraps up the Vulkan API in bite sized and easy to use classes that make constructing Vulkan pipelines as easy as creating a few objects.  

Spock uses The GL Math Library (GLM) for vectors and matrices and GLFW to handle the creation of windows and display surfaces.

## Repository Layout
This respository is layed out in a way that separates each component of the project.
The primary components of Spock are:
* Spock - API wrapper and demonstration
* ShaderPack - Vulkan shader packing for binary distribution
* ColdStorage - Windows tool to convert raw shader files to ShaderPacks
* Third Party Libraries
	* GLFW - Window and input management
	* GLM - Math library
	* Vulkan - Graphics library
* Tools - Useful tools like glslangvalidator.exe and ColdStorage.exe

## Project Dependencies
Each project in the solution already has all of its dependencies set up so there should be no need to edit additional
includes or libraries. However should the need arise below is the dependency list for each project.

### Spock Dependencies
* Vulkan
* GLFW
* GLM
* ShaderPack

### ShaderPack Dependencies
* Vulkan
* ~~GLM~~ GLM was originally included but is no longer necessary

###ColdStorage Dependencies
* ShaderPack
	* Vulkan (I haven't found a clever way to mask this dependency yet)


## Compiling Shaders
Since Spock makes use of ShaderPacks for easy loading of shaders the ColdStorage tool is required at the moment
until a raw Shader class is created or becomes necessary. This tool and the ShaderPack library are a work in progress
so things may not always work as expected.

To use ColdStorage open up a command prompt in the Tools folder and run "ColdStorage.exe -h" to get the help list.
Currently ColdStorage only supports individual files and multiple files. The directory compiler is not yet implemented.

Example syntax for vertex and fragment shader files called: ExampleVert.vert and ExampleFrag.frag.
`ColdStorage.exe files ExampleVert.vert ExampleFrag.frag`
