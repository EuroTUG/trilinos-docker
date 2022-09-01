# How to solve a linear system with Belos and Ifpack2?

## Didactic goals

This exercise will introduce you to the basics of preconditioned Krylov solvers in Trilinos. Therefore, you will work with Krylov solvers rom the `Belos` package and additive Schwarz methods from the `Ifpack2` package to be used as preconditioners.

You will learn

- how to create and configure a Krylov solver via `Belos`,
- how to create and configure an additive Schwarz method via `Ifpack2`,
- how to use the additive Schwarz method as a preconditioner for the outher Krylov method.

## Starting point

The Trilinos source code repository is located at `/opt/.../`. Required dependencies (such as `CMake`) are installed.

> Note: The Trilinos source code repository as well as all required dependencies are alerady part of the docker container for this Trilinos tutorial.
