#!/bin/bash

BUILD_TYPE=RELEASE

BASE_DIR=/opt/trilinos/source
INSTALL_DIR=/opt/trilinos/install

rm -rf CMake*

cmake \
    -D CMAKE_BUILD_TYPE:STRING=${BUILD_TYPE} \
    -D CMAKE_INSTALL_PREFIX:STRING=${INSTALL_DIR} \
    -D CMAKE_CXX_FLAGS:STRING="" \
    -D CMAKE_CXX_STANDARD:STRING=17 \
    -D CMAKE_VERBOSE_MAKEFILE:BOOL=OFF \
    \
    -D Trilinos_ASSERT_MISSING_PACKAGES:BOOL=ON \
    -D Trilinos_ENABLE_ALL_PACKAGES:BOOL=OFF \
    -D Trilinos_ENABLE_ALL_OPTIONAL_PACKAGES:BOOL=OFF \
    -D Trilinos_ENABLE_EXPLICIT_INSTANTIATION:BOOL=ON \
    -D Trilinos_ENABLE_Fortran:BOOL=OFF \
    -D Trilinos_VERBOSE_CONFIGURE:BOOL=OFF \
    \
    -D Trilinos_ENABLE_Amesos2:BOOL=ON \
      -D Amesos2_ENABLE_TESTS:BOOL=ON \
    -D Trilinos_ENABLE_Belos:BOOL=ON \
      -D Belos_ENABLE_Tpetra:BOOL=ON \
    -D Belos_ENABLE_EXAMPLES:BOOL=ON \
    -D Belos_ENABLE_TESTS:BOOL=ON \
    -D Trilinos_ENABLE_Galeri:BOOL=ON \
      -D Galeri_ENABLE_Xpetra:BOOL=ON \
      -D Trilinos_ENABLE_Ifpack2:BOOL=ON \
      -D Ifpack2_ENABLE_TESTS:BOOL=OFF \
    -D Trilinos_ENABLE_TESTS:BOOL=ON \
    -D Kokkos_ENABLE_SERIAL:BOOL=ON \
    -D Trilinos_ENABLE_Teuchos:BOOL=ON \
    -D Trilinos_ENABLE_Tpetra:BOOL=ON \
      -D Tpetra_ENABLE_DEPRECATED_CODE:BOOL=ON \
    -D Trilinos_ENABLE_Xpetra:BOOL=ON \
      -D Xpetra_ENABLE_Experimental:BOOL=ON \
      -D Xpetra_ENABLE_Kokkos_Refactor:BOOL=ON \
    -D Tpetra_ENABLE_TESTS:BOOL=ON \
    \
    -D TPL_ENABLE_MPI:BOOL=ON \
    -D TPL_ENABLE_BLAS:BOOL=ON \
    -D TPL_BLAS_LIBRARIES:FILEPATH=/usr/lib64/libblas.so.3 \
    -D TPL_ENABLE_LAPACK:BOOL=ON \
    -D TPL_LAPACK_LIBRARIES:FILEPATH=/usr/lib64/liblapack.so.3 \
    \
    ${BASE_DIR}
