# Tutorial

## 1. Overview

### 1.1 Prerequisites

In order to understand CMake more easily and better, the users are strongly recommended to do the following things before learning CMake.

Firstly, get some basic understanding of [the `make` build tool](https://en.wikipedia.org/wiki/Make_(software)). Although CMake and `make` are essentially different, I feel CMake gets a lot of ideas from `make` so knowing `make` will help learning CMake. Specifically, learn what "targets" are, what "rules" are, and how to specify the dependencies between different targets in order to build the targets successfully.

Secondly, get some basic understanding of object-oriented programming (OOP). CMake is implemented in C++, so some concepts of CMake are quite similar to the concept of "classes" which have "properties" to define what they are and also "functions" to define what they can do. One example is the "target" in CMake, which we will explain later.

Finally, view CMake as a scripting language. This not only helps you to understand why CMake can define variables and functions but also helps to understand why CMake modules (see [cmake-modules(7)](https://cmake.org/cmake/help/latest/manual/cmake-modules.7.html)) are part of CMake.

### 1.2 The Problems that CMake Solves

CMake has many features because it aims to solving many problems. Most of the features revolve around building and testing the code, but CMake can also pack up the built artifacts and distribute them. This section lists the problems that CMake handles. Knowing these problems before diving into CMake details can provide you a view of the "whole forest" of CMake. Later, whenever you are learning a specific CMake feature, you will always know where you are in this forest without getting lost.

The problems are labeled with the indices such as "P1", "P2", and "P3". The indices make references easier.

- **P1: Detect the current build host system's various features.** Many projects conditionally compile the code according to the presence of a particular feature. For example, if the build host system provides native threading library, the code will directly use the native threading library; otherwise, it may use its own threading library.
- **P2: Build a library or an executable that may link to other libraries.** This problem can be divided into smaller problems:
  - **P2-1: How to define the rules that build the library or the executable.**
  - **P2-2: How to effectively manage the rules.** If a library is the used in multiple places, we need to make sure the library is linked with the correct parameters everywhere. This becomes more challenging when the project and the library keep growing more complex. CMake provides the feature "transitive usage requirements" to solve this problem.
  - **P2-3: How to find a library if it comes from an external package.**
- **P3: Specify the build configuration (e.g., `Debug` vs `Release`).**
- **P4: Test the built artifacts.**
- **P5: Create an installer so others can install the package.**

The subsequent sections will discuss CMake features and refer back here to show what problems a feature solves.

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

## Appendix A: Pitfalls

When reading the help document, note whether a command requires a `target` or a `file`, or something else. For example, the help document of the command `install` says:

```
install(TARGETS <target>... [...])
install(IMPORTED_RUNTIME_ARTIFACTS <target>... [...])
install({FILES | PROGRAMS} <file>... [...])
install(DIRECTORY <dir>... [...])
install(SCRIPT <file> [...])
install(CODE <code> [...])
install(EXPORT <export-name> [...])
install(RUNTIME_DEPENDENCY_SET <set-name> [...])
```

Note that `install` accepts different types of arguments for different forms. When it is a `target`, you can specify an earlier defined target and CMake will find it automatically for you; when it is a `file`, you need to specify a file path. If you try to use a `target`, you will get unexpected results.

For example, if you have defined a target `main_not_using`:

```
add_executable(
    main_not_using
    "main-not-using.cpp"
)
```

and you want to install it, using `install(PROGRAMS main_not_using TYPE BIN)` will result in the error "No such file or directory" because `main_not_using` is not interpreted as a target but a relative path to the file. You need to use `install(PROGRAMS $<TARGET_FILE:main_not_using> TYPE BIN)`.

## References

- [1] [`add_executable()`](https://cmake.org/cmake/help/v3.21/command/add_executable.html)
- [2] [`add_library()`](https://cmake.org/cmake/help/v3.21/command/add_library.html)
- [3] [`add_custom_target()`](https://cmake.org/cmake/help/v3.21/command/add_custom_target.html)
- [4] [`add_custom_command()`](https://cmake.org/cmake/help/v3.21/command/add_custom_command.html)
  - [4.1] [Generating Files](https://cmake.org/cmake/help/v3.21/command/add_custom_command.html#generating-files)
  - [4.2] [Build Events](https://cmake.org/cmake/help/v3.21/command/add_custom_command.html#build-events)
- [5] [cmake-properties(7)](https://cmake.org/cmake/help/latest/manual/cmake-properties.7.html)
