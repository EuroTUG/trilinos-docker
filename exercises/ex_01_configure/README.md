# How to configure Trilinos?

## Didactic goals

This exercise will guide you through the configuration process of Trilinos.

You will learn

- how to invoke the `cmake` command to configure the Trilinos build with your choice of packages,
- how to incorporate this invocation into a congfigure script for reproducibility,
- to make first steps towards enabling / disabling packages to satisfy dependencies between packages,
- how to fix common errors related to including Third Party Library into the Trilinos build.

## Starting point

The Trilinos source code repository is located at `/opt/trilinos/`.
Required dependencies (such as `CMake`, `SuiteSparse`, ...) are pre-installed in the docker container.

> Note: The Trilinos source code repository as well as all required dependencies are alerady part of the docker container for this Trilinos tutorial.

## Preparing the directory structure

1. Navigate to base directory for this exercise

   ```bash
   cd /opt/trilinos_demo/exercises/ex_01_configure
   ```

1. Create the build directory via

   ```bash
   mkdir trilinos_build
   ```

1. Create the install directory via

   ```bash
   mkdir trilinos_install
   ```

## Configure Trilinos

> **Background information on the configure scirpt:** Trilinos will be configured by invoking the `cmake` command.
The specific Trilinos configuration is managed via a set of options to be passed to the `cmake` command.
For easier handling, documentation and reproducibility, we will write the `cmake` command within a configure script.
To start the configuration process, we will then just execute this script.

1. In the base directory `/opt/trilinos_demo/exercises/ex_01_configure`, create a file `do-configure-trilinos` using your favorite text editor.

1. To make this script executable as a `bash` script, insert this into the first line:

   ```
   #!/bin/bash
   ```

1. Define a environment variable to set the build type:

   ```
   BUILD_TYPE=RELEASE
   ```

1. Define these environment variables to point to the Trilinos source, build, and install directories:

   ```
   TRILINOS_SOURCE=/opt/trilinos/source
   TRILINOS_BUILD=/opt/trilinos_demo/exercises/ex_01_configure/trilinos_build
   TRILINOS_INSTALL=/opt/trilinos_demo/exercises/ex_01_configure/trilinos_install
   ```

1. Write the `cmake` command:

   > **Remarks:**
   >
   > 1. We recommend to structure the list of options within the `cmake` comand into four blocks:
   > build settings, general Trilinos settings, Trilinos packages configuration, Third Pary Libraries (TPLs).
   >
   > 1. We use the end-of-line symbol `\` to allow for line breaks within the `cmake` command in order to improve readibility.

	```
	cmake \
      -D CMAKE_BUILD_TYPE:STRING=${BUILD_TYPE} \
      -D CMAKE_INSTALL_PATH:STRING=${TRILINOS_INSTALL} \
      -D CMAKE_CXX_FLAGS:STRING="" \
      -D CMAKE_CXX_STANDARD:STRING=14 \
      -D CMAKE_VERBOSE_MAKEFILE:BOOL=OFF \
      \
      -D Trilinos_ASSERT_MISSING_PACKAGES:BOOL=ON \
      -D Trilinos_ENABLE_ALL_PACKAGES:BOOL=OFF \
      -D Trilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=OFF \
      -D Trilinos_ENABLE_EXPLICIT_INSTANTIATION:BOOL=ON \
      -D Trilinos_ENABLE_Fortran:BOOL=OFF \
      -D Trilinos_VERBOSE_CONFIGURE:BOOL=OFF \
      \
      -D Trilinos_ENABLE_Belos:BOOL=ON \
      -D Trilinos_ENABLE_Galeri:BOOL=ON \
      -D Trilinos_ENABLE_Ifpack2:BOOL=ON \
        -D Trilinos_ENABLE_TESTS:BOOL=ON \
      -D Trilinos_ENABLE_Kokkos:BOOL=OFF \
        -D Kokkos_ENABLE_SERIAL:BOOL=ON \
      -D Trilinos_ENABLE_Tpetra:BOOL=ON \
      \
      -D TPL_ENABLE_MPI:BOOL=ON \
      -D TPL_ENABLE_BLAS:BOOL=ON \
      -D TPL_ENABLE_LAPACK:BOOL=ON \
        -D TPL_LAPACK_LIBRARIES:FILEPATH=/usr/lib64/liblapack.so.3 \
      -D TPL_ENABLE_UMFPACK:BOOL=ON \
        -D UMFPACK_INCLUDE_DIRS:FILEPATH=/opt/suitesparse/install/include \
        -D UMFPACK_LIBRARY_DIRS:FILEPATH=/opt/suitesparse/install/lib \
      \
      ${TRILINOS_SOURCE}
	```

1. Navigate to the build directory: `cd /opt/trilinos_demo/exercises/ex_01_configure/trilinos_build`

1. Run the configure script: `source ../do-configure-trilinos`

1. The configuration process will return an error like this:

   ```
   CMake Error at cmake/tribits/core/package_arch/TribitsAdjustPackageEnables.cmake:83 (message):
      ***
      *** ERROR: Setting Trilinos_ENABLE_Ifpack2=OFF which was 'ON' because Ifpack2 has a required library dependence on disabled package KokkosKernels!
      ***

   Call Stack (most recent call first):
     cmake/tribits/core/package_arch/TribitsAdjustPackageEnables.cmake:166 (tribits_private_print_disable)
     cmake/tribits/core/package_arch/TribitsAdjustPackageEnables.cmake:189 (tribits_private_print_disable_required_package_enable)
     cmake/tribits/core/package_arch/TribitsAdjustPackageEnables.cmake:377 (tribits_private_disable_required_package_enables)
     cmake/tribits/core/package_arch/TribitsAdjustPackageEnables.cmake:1140 (tribits_disable_forward_required_dep_packages)
     cmake/tribits/core/package_arch/TribitsGlobalMacros.cmake:1550 (tribits_adjust_package_enables)
     cmake/tribits/core/package_arch/TribitsProjectImpl.cmake:161 (tribits_adjust_and_print_package_dependencies)
     cmake/tribits/core/package_arch/TribitsProject.cmake:93 (tribits_project_impl)
     CMakeLists.txt:109 (TRIBITS_PROJECT)


   -- Configuring incomplete, errors occurred!
   ```

   In particular, `Ifpack2` cannot be activated without `Kokkos` being activated.

1. Activate `Kokkos` by changing the `Kokkos` activation bool in the configure script from `OFF` to `ON`.

1. Run the configure script again.

1. The configuration process will return an error like this:

   ```
   Processing enabled TPL: BLAS (enabled explicitly, disable with -DTPL_ENABLE_BLAS=OFF)
   -- BLAS_LIBRARY_NAMES='blas blas_win32'
   -- Searching for libs in BLAS_LIBRARY_DIRS=''
   -- Searching for a lib in the set "blas blas_win32":
   --   Searching for lib 'blas' ...
   --   Searching for lib 'blas_win32' ...
   -- NOTE: Did not find a lib in the lib set "blas blas_win32" for the TPL 'BLAS'!
   -- ERROR: Could not find the libraries for the TPL 'BLAS'!
   -- TIP: If the TPL 'BLAS' is on your system then you can set:
        -DBLAS_LIBRARY_DIRS='<dir0>;<dir1>;...'
      to point to the directories where these libraries may be found.
      Or, just set:
        -DTPL_BLAS_LIBRARIES='<path-to-libs0>;<path-to-libs1>;...'
      to point to the full paths for the libraries which will
      bypass any search for libraries and these libraries will be used without
      question in the build.  (But this will result in a build-time error
      if not all of the necessary symbols are found.)
   -- ERROR: Failed finding all of the parts of TPL 'BLAS' (see above), Aborting!

   -- NOTE: The find module file for this failed TPL 'BLAS' is:
        /opt/trilinos/source/cmake/tribits/common_tpls/FindTPLBLAS.cmake
      which is pointed to in the file:
        /opt/trilinos/source/TPLsList.cmake

   TIP: Even though the TPL 'BLAS' was explicitly enabled in input,
   it can be disabled with:
     -DTPL_ENABLE_BLAS=OFF
   which will disable it and will recursively disable all of the
   downstream packages that have required dependencies on it.
   When you reconfigure, just grep the cmake stdout for 'BLAS'
   and then follow the disables that occur as a result to see what impact
   this TPL disable has on the configuration of Trilinos.

   CMake Error at cmake/tribits/core/package_arch/TribitsProcessEnabledTpl.cmake:144 (message):
     ERROR: TPL_BLAS_NOT_FOUND=TRUE, aborting!
   Call Stack (most recent call first):
     cmake/tribits/core/package_arch/TribitsGlobalMacros.cmake:1581 (tribits_process_enabled_tpl)
     cmake/tribits/core/package_arch/TribitsProjectImpl.cmake:196 (tribits_process_enabled_tpls)
     cmake/tribits/core/package_arch/TribitsProject.cmake:93 (tribits_project_impl)
     CMakeLists.txt:109 (TRIBITS_PROJECT)


   -- Configuring incomplete, errors occurred!
   ```

   In particular, the complied lib files of the TPL `BLAS` have not been found.

1. Point the build system to the `BLAS` lib files by specifying the location of the compiled `BLAS` lib file to the `cmake` command as follows:

   ```
   -D TPL_ENABLE_BLAS:BOOL=ON \
     -D TPL_BLAS_LIBRARIES:FILEPATH=/usr/lib64/libblas.so.3 \
   -D TPL_ENABLE_LAPACK:BOOL=ON \
   ```

1. Run the configure script again. Now, you the sript should pass and finish with the following output:

   ```
   Set up for creating a distribution ...


   Finished configuring Trilinos!

   -- Configuring done
   -- Generating done
   -- Build files have been written to: /opt/trilinos_demo/exercise_configure/trilinos_build
   ```

## Build and install Trilinos

1. Navigate to the build directory: `cd /opt/trilinos_demo/exercises/ex_01_configure/trilinos_build`
1. Build via `make -j <numProc>` with `<numProc>` beeing the number of cores used for a parallel build.
1. After a successful build, install via `make install`.
1. Inspect the content of the build and install directory to see the result of the build and installation process.

> **Hint:** Build and install can also be invoked by a single command line instruction: `make -j <numProc> install`
