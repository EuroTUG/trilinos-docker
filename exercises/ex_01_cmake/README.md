# How to build an app using Trilinos?

In this exercise, you will work on a small app that runs in parallel and prints some information about the MPI communicator to the screen.

## Didactic goals

This exercise will guide you through the setup of a application that uses Trilinos.

You will learn

- how to include Trilinos in your `CMakeLists.txt`,
- how to use Trilinos objects in your application code.

## Starting point

To decouple this exercise from the previous `ex_01_configure`,
let's now use the pre-installed Trilinos.

> Note: Since you will invoke CMake's `find_package()` to locate the Trilinos installation,
you will not need to specify the exact
The Trilinos installation is located at `/opt/trilinos/install/`.

This exercise comes with a small application and some source code in `/opt/trilinos_demo/exercises/ex_01_cmake/src/`.

## Preparing the directory structure

1. Navigate to base directory for this exercise

   ```bash
   cd /opt/trilinos_demo/exercises/ex_01_cmake/
   ```

1. Create the build directory via

   ```bash
   mkdir build
   ```

## Creating the CMake project (without Trilinos)

> In this part of the exercise, you will work with an existing `CMakeLists.txt` file, where you will fill in imporant parts yourself.
The file is located at `/opt/trilinos_demo/exercises/ex_01_cmake/src/CMakeLists.txt`. Places, where modifications are required, are encapsulated by the lines

```cmake
### START OF TODO: <Short instruction> ###
...
### END OF TODO: <Short instruction> ###
```

where the `<Short instruction>` provides a summary of the steps to be done.

1. Open the file `/opt/trilinos_demo/exercises/ex_01_cmake/src/CMakeLists.txt` in your favorite text editor/IDE.
1. At `### START OF TODO: Set minimum CMake version`, set the minimum required CMake version by adding

   ```cmake
   cmake_minimum_required(VERSION 3.17.1)
   ```

   > _Background:_ Trilinos requires at least CMake version 3.17.

1. At `### START OF TODO: : Declare project ###`, declare the CMake project by adding

   ```cmake
   project(ex_01_configure NONE)
   ```

   > _Note:_ Pass the option `NONE` to not declare the language and compilers right away.
   We will do so later after having invoked `find_package()` to locate the Trilinos installation.
   Then, we can use the same compiler/linker settings for the application.

## Building the application code - Part I

> In this part of the exercise, you will work with an existing `main.cpp` file, where you will fill in imporant parts yourself.
The file is located at `/opt/trilinos_demo/exercises/ex_01_cmake/src/main.cpp`. Places, where modifications are required, are encapsulated by the lines

```cpp
/* START OF TODO: <Short instruction> */
...
/* END OF TODO <Short instruction> */
```

where the `<Short instruction>` provides a summary of the steps to be done.

1. Navigate to `/opt/trilinos_demo/exercises/ex_01_cmake/` and create a build directory via `mkdir build`.
1. Navigate into the `build/` directory.
1. Configure the project with the existing configure script via `source ../do-configure-ex-01-cmake`.
1. Build the app via `make`.
1. Run the app via `./ex-01-cmake`
   You will obtain the following output:

   ```
   This is output from processor -1.
   ```
1. Run the app on four MPI ranks via `mpirun -np 4 ./ex-01-cmake`.
   You will obtain the following output:

   ```
   This is output from processor -1.
   This is output from processor -1.
   This is output from processor -1.
   This is output from processor -1.
   ```

   Since you run the code on four MPI ranks, you see the same output four times.
   So far, we have not setup the MPI environment.
   Hence, each rank just prints an default value (specific to this example), but not its correct ID.

1. To setup the parallel environemnt,

   - at `/* START OF TODO: Tpetra header inclusion */`, include the header `#include <Tpetra_Core.hpp>`.
   - at `/* START OF TODO: Setup parallel environment */, insert the line
      ```c++
      Tpetra::ScopeGuard tpetraScope(&argc, &argv);
      ```
      to ask `Tpetra` for setting up MPI (and Kokkos if necessary).

1. Re-compile. You will get an error message similar to

   ```
   In file included from /usr/local/include/Teuchos_TimeMonitor.hpp:69,
                 from /usr/local/include/Teuchos_CommUtilities.hpp:45,
                 from /usr/local/include/Teuchos_CommHelpers.hpp:46,
                 from /usr/local/include/Tpetra_ConfigDefs.hpp:94,
                 from /usr/local/include/Tpetra_Core.hpp:52,
                 from /opt/trilinos_demo/exercises/ex_01_cmake/src/main.cpp:12:
   /usr/local/include/Teuchos_Time.hpp:56:10: fatal error: mpi.h: No such file or directory
   #include <mpi.h>
          ^~~~~~~
   compilation terminated.
   ```

   The compiler is looking for Trilinos files/objects, but we have not added Trilinos to this prohect yet.
   Hence, let's go back to `CMakeLists.txt` to enable Trilinos.

## Adding Trilinos to the CMake project

1. At `### START OF TODO: Get Trilinos`,
search for the Trilinos installation and specifically ask for the `Teuchos` and `Tpetra` packages.
To do so, insert

   ```cmake
   find_package(Trilinos REQUIRED COMPONENTS Teuchos Tpetra)
   ```

   into `CMakeLists.txt`.

## Building the application code - Part II

1. Clear the `build/` directory and configure by running `source ../do-configure-ex-01-cmake` within the `build/` directory.
1. Compile.

The app should compile properly.
Since you have not changed the definition of the output so far, the output will still look the same as before.

To now print information about the MPI communicator, you first have to grab it.

1. At `/* START OF TODO: Teuchos header inclusion */`, include the header `#include <Teuchos_Comm.hpp>`.

1. At `/* START OF TODO: Get communicator object */`, ask the `Teuchos` package to hand you the communicator object.
Therefore, insert

   ```c++
   Teuchos::RCP<const Teuchos::Comm<int>> comm = Tpetra::getDefaultComm();
   ```

   > For more details on `Teuchos::RCP` or `Teuchos::Comm`, please consult the [`Teuchos` documentation](https://trilinos.org/docs/dev/packages/teuchos/doc/html/index.html).

1. At `/* START OF TODO: Extract info from communicator */`, query the communicator about its size and rank ID by inserting

   ```c++
   numProcs = comm->getSize();
   myRank = comm->getRank();
   ```

1. Compile.

1. Run the app in serial via `./ex_01_cmake` to obtain the following output:

   ```
   Total number of processes: 1
   This is output from processor 0.
   ```

1. Run the app in parallel on four MPI ranks `mpirun -np 4 ./ex_01_cmake`. You will obtain output similar to the following:

   ```
   Total number of processes: 4
   This is output from processor 0.
   This is output from processor 1.
   This is output from processor 2.
   This is output from processor 3.
   ```

   > _Note:_ The order of the output can be shuffled, since there's no coordination among processes. That's fine!

Congratulations! You have built a first application based on Trilinos.
