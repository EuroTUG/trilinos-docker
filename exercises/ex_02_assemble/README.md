# How to assemble a linear system in Tpetra?

In this exercise, you will assemble a linear system arising from the finite element discretization of a one-dimensional Poisson problem (wihtout boundary conditions for simplicity).
The matrix will exhibit the well-know [-1, 2, -1] pattern in each row (except for the first and last row).

## Didactic goals

This exercise will introduce you to basic concepts of the sparse linear algebra package `Tpetra`a `Tpetra::Map` and will show you how to assemble a linear system (matrix and right-hand side vector) using `Tpetra` objects.

You will learn

- how to create a `Tpetra::Map` object to represent the parallel distribution of data,
- how to create and fill a vector,
- how to create and fill a sparse matrix.

## Starting point

The Trilinos source code repository is located at `/opt/.../`. Required dependencies (such as `CMake`) are installed.

> Note: The Trilinos source code repository as well as all required dependencies are alerady part of the docker container for this Trilinos tutorial.

## Preparing the directory structure

1. Navigate to base directory for this exercise

   ```bash
   cd /opt/trilinos_demo/exercises/ex_02_assemble/
   ```

1. Create the build directory via

   ```bash
   mkdir build
   ```

## Configure the build system

1. Navigate into the build directory
1. Run the pre-existing configure script:

   ```bash
   source ../do-configure-ex-02
   ```

## Complete the application's source code

In this part of the exercise, you will work with an existing `main.cpp` file, where you will fill in imporant parts yourself.
The file is located at `/opt/trilinos_demo/exercises/ex_02_assemble/src/main.cpp`.
Places, where modifications are required, are encapsulated by the lines

```cpp
/* START OF TODO: <Short instruction> */
...
/* END OF TODO <Short instruction> */
```

1. At `/* START OF TODO: Create map */`, define the global number of entries in the map and construct the map:

   ```cpp
   const global_ordinal_type indexBase = 0;
   RCP<const map_type> map = rcp(new map_type(numGblIndices, indexBase, comm));
   ```

   Make sure to include the necessary header file `Teuchos_Map.hpp`.

1. At `/* START OF TODO: Create empty matrix */`, create an empty `Tpetra::CrsMatrix<...>` with an estimated three entries per row:
   ```cpp
   RCP<crs_matrix_type> A = rcp(new crs_matrix_type (map, 3));
   ```

Now, you need to fill the matrix, which is done row by row.
Therefore, each process loops over its rows using the local index `lclRow`.

```cpp
for (local_ordinal_type lclRow = 0; lclRow < static_cast<local_ordinal_type>(numMyElements); ++lclRow) {
   // Convert local index to global index
   const global_ordinal_type gblRow = map->getGlobalElement(lclRow);
   // Now, fill global row with id 'gblRow'
   ...
}
```

Since one needs to insert values based on global indices (due to not having computed a column map so far),
local indices nedd to be converted to global indices by querying the `map`.

1. At `/* START OF TODO: convert local to global index */`, insert the conversion from local to global indices:

   ```cpp
   const global_ordinal_type gblRow = map->getGlobalElement(lclRow);
   ```
1. At `/* START OF TODO: Fill first row */`, fill the first row of the matrix with

   - 2.0 in the first column
   - -1.0 in the second column

   > _Note:_ To make sure, that the data type matches the scalar type used in Tpetra, these scalar values are not typed as `2.0` and `-1.0`, but are constructed via
   >
   > ```cpp
   > const scalar_type two = static_cast<scalar_type>(2.0);
   > const scalar_type negOne = static_cast<scalar_type>(-1.0);
   > ```

   Insert the first row using the following code snippet:
   ```c++
   A->insertGlobalValues(gblRow, tuple<global_ordinal_type>(gblRow, gblRow + 1), tuple<scalar_type>(two, negOne));
   ```
   where you pass the global row id `gblRow`, the list of global column indices and the list of values to be inserted.

1. At `/* START OF TODO: Fill last row */`, fill the last row of the matrix with

   - -1.0 in the second-to-last column
   - 2.0 in the last column

   via

   ```cpp
   A->insertGlobalValues(gblRow, tuple<global_ordinal_type>(gblRow - 1, gblRow), tuple<scalar_type>(negOne, two));
   ```

1. At `/* START OF TODO: Fill intermediate rows */`, fill all intermediate rows with

   - -1.0 left of the diagonal element
   - 2.0 at the diagonal element
   - -1.0 right of the diagonal element

   via

   ```cpp
   A->insertGlobalValues(gblRow,
       tuple<global_ordinal_type>(gblRow - 1, gblRow, gblRow + 1),
       tuple<scalar_type>(negOne, two, negOne));
   ```

Now, you are done with filling the matrix.

1. At `/* START OF TODO: Fill complete */`, indicate that you are done with inserting values into the matrix by calling:

     ```cpp
     A->fillComplete(map, map);
     ```

1. At `/* START OF TODO: Fill right-hand side */`,
create the right-hand side vector `b` with a suitable map (i.e. the range map of `A`)
and fill it with all ones:
   ```cpp
   RCP<vec_type> b = rcp(new vec_type(A->getRangeMap()));
   b->putScalar(Teuchos::ScalarTraits<scalar_type>::one());
   ```

   > _Note:_ Use `Teuchos::ScalarTraits<scalar_type>::one()` instead of `1.0` to make sure, that the data type matches the scalar type used in Tpetra.

## Build and run the app

1. Compile the app via `make`.
1. Run the app in serial via `./ex_02_assemble`.
1. Run the app in parallel on `<numProc>` MPI ranks via `mpirun -np <numProc> ./ex_02_assemble`.
