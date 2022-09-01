# How to assemble a linear system in Tpetra?

## Didactic goals

This exercise will introduce you to basic concepts of the sparse linear algebra package `Tpetra`a `Tpetra::Map` and will show you how to assemble a linear system (matrix and right-hand side vector) using `Tpetra` objects. 

You will learn

- how to create a `Tpetra::Map` object to represent the parallel distribution of data,
- how to create and fill a vector,
- how to create and fill a sparse matrix.

## Starting point

The Trilinos source code repository is located at `/opt/.../`. Required dependencies (such as `CMake`) are installed.

> Note: The Trilinos source code repository as well as all required dependencies are alerady part of the docker container for this Trilinos tutorial.

