# CMake Explained: A Tutorial of Modern CMake

## 1. Overview

### 1.1 Current Status

This tutorial is still a **work in progress**.

### 1.2 Current Version

This tutorial uses [semantic versioning](https://semver.org/) and is currently `0.2.0`. See [`CHANGELOG.md`](./CHANGELOG.md) for more details in each version.

### 1.3 Prerequisites

In order to understand CMake more easily and better, the users are strongly recommended to learn the following topics:
- Basic object-oriented programming (OOP) knowledge, including:
  - What is an object?
  - What is a property of an object?
  - What is a method (i.e., action) of an object?
- Understand that reusing code is a fundamental software engineering principle. More specifically:
  - Common code is extracted into functions or modules for reusability.
  - A programming language typically provides some "include" or "import" (or both) mechanism to make the reusable code available to the current development.

### 1.4 How CMake Helps with Building?

I'm using CMake 3.21.3.

The typical steps of building a library or a program are:
- Install pre-requisites of building the program.
- Detect the current build host's various system features (i.e., system introspection).
- Find the external libraries or programs that the target program depends on.
- Specify the build configuration (e.g., `Debug` vs `Release`).
- Build the internal libraries.
- Build the executable programs (linking statically or dynamically).
- Test the built artifacts to make sure things are built correctly.
- Create an installer.
- Distribute the installer so others can install it.

The various CMake features are used in different steps.

## 2. Target

### 2.1 Overview

A `target` is the artifact we want to build. For example, an executable, a static library (a `.a` file).

There are three kinds of targets:

| Target Type | Defined By |
|:-----------:|:----------:|
| Executable target | `add_executable()` [1] |
| Library target | `add_library()` [2] |
| Custom target | `add_custom_target()` [3] |

### 2.2 Library Target

| Library Target Type | Defined By | Description |
|:-------------------:|:-----------|:------------|
| Static | `add_library(lib_name STATIC [source files])` | A static library (e.g., a `.a` file) |
| Shared | `add_library(lib_name SHARED [source files])` | A shared library (e.g., a `.so` file) |
| Module | `add_library(lib_name MODULE [source files])` | Used as a runtime plugin and is not linked to (i.e., not used in the right-hand-side of the `target_link_libraries()` command) |

### 2.3 Custom Target and `add_custom_target()`

A **custom target** is an abstract entity that has no intention to generate any output file, although the side effect of the command can be a file, as `add_custom_target` [3] documentation explains:

> Adds a target with the given name that executes the given commands. The target has **no output file** and is **always** considered out of date even if the commands try to create a file with the name of the target. Use the `add_custom_command()` command to generate a file with dependencies. By default nothing depends on the custom target. By default nothing depends on the custom target. Use the `add_dependencies()` command to add dependencies to or from other targets.

### 2.4 `add_custom_command()`

`add_custom_command()`, according to [4], has two purposes:
- Generating a file.
- Triggering an event when a target is built.

In order to generate a file, `add_custom_command()` takes the following form [4.1]:

```cmake
add_custom_command(OUTPUT output1 [output2 ...]
                   COMMAND command1 [ARGS] [args1...]
                   ...
                   ...
```

The second form "adds a custom command to a target such as a library or executable" [4.2]:

```cmake
add_custom_command(TARGET <target>
                   PRE_BUILD | PRE_LINK | POST_BUILD
                   COMMAND command1 [ARGS] [args1...]
                   ...
                   ...
```

As [4.2] explains:

> This is useful for performing an operation before or after building the target. The command becomes part of the target and will only execute when the target itself is built. **If the target is already built, the command will not execute**.

### 2.5 My Opinions

I think the name of `add_custom_command()` has become confusing. Probably at the very beginning, `add_custom_command()` was only used to run a custom command. As `CMake` evolved, it started to support the current two purposes. I think it's better to name the two functions separately: `add_custom_file()` (for `add_custom_command(OUTPUT)`) and `trigger_build_event()` (for `add_custom_command(TARGET)`).

## 3. Properties

### 3.1 What Is A Property?

`cmake-properties(7)` [5] lists all the supported properties. Unfortunately, `CMake` documentation fails to explain what a property is but directly starts to use it as if the readers are already quite familiar with it.

So far, I think the best analogue of a "property" in `CMake` is a data member of a class in object-oriented programming (OOP).

In OOP, a class is an abstract concept that describes the properties and behaviors of a whole type of objects. When we design a system in the object-oriented methodology, we identify the entities inside the target system that interact with each other. Each type of identity is a potential class, and each class has some properties to describe what they are. For example, a simple drawing application may allow the users to draw shapes such as rectangle and circle. A "rectangle" may has three properties: the coordinates that describe where the current rectangle is located; the length of its longer sides; the length of its shorter sides. If the application supports coloring, a rectangle may have additional properties such as the filling color and the border color.

In `CMake`, a "property" describes a characteristic of an "entity" that `CMake` cares about. `cmake-properties(7)` lists the entities that `CMake` cares about, which are:
- `CMake` itself (i.e., the "global scope")
- Directories
- Targets
- Tests
- Source Files
- Cache Entries
- Installed Files

I haven't read any line of `CMake` source code, but I guess these entities are defined as classes somewhere, such as:

```cpp
class Directory
{
  // ...
};

class Target
{
  // ...
};

class SourceFile
{
  // ...
};

...
```

A property defines what characteristic the current entity has. For example, one target may be built as a library for MacOS, then its [`FRAMEWORK`](https://cmake.org/cmake/help/latest/prop_tgt/FRAMEWORK.html) property should be set `TRUE`; another target requires to use a minimal CUDA/C++ standard, then its [`CUDA_STANDARD`](https://cmake.org/cmake/help/latest/prop_tgt/CUDA_STANDARD.html) must be set to an appropriate value.

### 3.2 Property Setters/Getters

`CMake` provides the following commands to manipulate the properties, as if they are the "setters" and "getters" of a class:

| Command | Description |
|--------:|:------------|
| `get_cmake_property` | Get a global property of the CMake instance. |
| `get_directory_property` | Get a property of `DIRECTORY` scope. |
| `get_property` | Gets one property from one object in a scope. |
| `set_property` | Sets one property on zero or more objects of a scope. |
| `define_property` | Defines one property in a scope for use with the `set_property()` and `get_property()` commands. |
| `get_source_file_property` | Get a property for a source file. |
| `get_target_property` | Get a property from a target. |
| `get_test_property` | Get a property of the test. |

### 3.3 Properties of the Same Name

Note that properties of the same name can exist in different scopes. For example, the property name `INCLUDE_DIRECTORIES` appear in the scopes of `Directories`, `Targets`, and `Source Files`, but they are three **different** properties. Therefore, when you read the document and see a command updates a property, you must figure out which property of which scope the command updates.

## References

- [1] [`add_executable()`](https://cmake.org/cmake/help/v3.21/command/add_executable.html)
- [2] [`add_library()`](https://cmake.org/cmake/help/v3.21/command/add_library.html)
- [3] [`add_custom_target()`](https://cmake.org/cmake/help/v3.21/command/add_custom_target.html)
- [4] [`add_custom_command()`](https://cmake.org/cmake/help/v3.21/command/add_custom_command.html)
  - [4.1] [Generating Files](https://cmake.org/cmake/help/v3.21/command/add_custom_command.html#generating-files)
  - [4.2] [Build Events](https://cmake.org/cmake/help/v3.21/command/add_custom_command.html#build-events)
- [5] [cmake-properties(7)](https://cmake.org/cmake/help/latest/manual/cmake-properties.7.html)
