# How to configure Trilinos?

## Didactic goals

This exercise will guide you through the configuration process of Trilinos.

You will learn

- how to invoke the `cmake` command to configure the Trilinos build with your choice of packages.
- how to incorporate this invocation into a congfigure script for reproducibility.
- to make first steps towards enabling / disabling packages to satisfy dependencies between packages.
- how to fix common errors related to including Third Party Library into the Trilinos build.

## Starting point

The Trilinos source code repository is located at `/opt/.../`. Required dependencies (such as `CMake`) are installed.

> Note: The Trilinos source code repository as well as all required dependencies are alerady part of the docker container for this Trilinos tutorial.
