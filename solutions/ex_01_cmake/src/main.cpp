/* This example shows the very first steps to include Trilinos into
 * an application code.
 */
#include <iostream>
#include <sstream>

/* START OF TODO: Teuchos header inclusion */
#include <Teuchos_Comm.hpp>
/* END OF TODO: Teuchos header inclusion*/

/* START OF TODO: Tpetra header inclusion */
#include <Tpetra_Core.hpp>
/* END OF TODO: Tpetra header inclusion */

int main(int argc, char *argv[]) {
  // Start up MPI, if using MPI.  Trilinos doesn't have to be built
  // with MPI; it's called a "serial" build if you build without MPI.
  // Tpetra::ScopeGuard hides this implementation detail.
  /* START OF TODO: Setup parallel environment */
  Tpetra::ScopeGuard tpetraScope(&argc, &argv);
  /* END OF TODO: Setup parallel environment */

  // Get a pointer to the communicator object representing
  // MPI_COMM_WORLD.  The function knows whether or not we built with
  // MPI support.  If we didn't build with MPI, we'll get a
  // "communicator" with size 1, whose only process has rank 0.
  /* START OF TODO: Get communicator object */
  Teuchos::RCP<const Teuchos::Comm<int>> comm = Tpetra::getDefaultComm();
  /* END OF TODO: Get communicator object */

  // Get my process' rank, and the total number of processes.
  // Equivalent to MPI_Comm_rank resp. MPI_Comm_size.
  int numProcs = -99;
  int myRank = -1;
  /* START OF TODO: Extract info from communicator */
  numProcs = comm->getSize();
  myRank = comm->getRank();
  /* END OF TODO: Extract info from communicator */

  // Write some output, first only on proc 0, then on all procs
  if (myRank == 0) {
    std::cout << "Total number of processes: " << numProcs << std::endl;
  }
  std::stringstream msg; msg << "This is output from processor " << myRank << ".\n";
  std::cout << msg.str();

  // Tpetra::ScopeGuard's destructor calls MPI_Finalize, if its constructor
  // called MPI_Init.  Likewise, it calls Kokkos::finalize, if its
  // constructor called Kokkos::initialize.

  return 0;
}
