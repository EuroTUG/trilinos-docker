/* This example shows the very first steps to assemble a linear system in Tpetra.
 */

#include <Amesos2.hpp>

#include <Teuchos_Array.hpp>
#include <Teuchos_RCP.hpp>
#include <Teuchos_ScalarTraits.hpp>

#include <Tpetra_Core.hpp>
#include <Tpetra_CrsMatrix.hpp>
#include <Tpetra_Map.hpp>
#include <Tpetra_MultiVector.hpp>
#include <Tpetra_Vector.hpp>
#include <Tpetra_Version.hpp>

int main(int argc, char *argv[]) {
  using Teuchos::RCP;
  using Teuchos::rcp;
  using Teuchos::tuple;

  using crs_matrix_type = Tpetra::CrsMatrix<>;
  using map_type = Tpetra::Map<>;
  using multivec_type = Tpetra::MultiVector<>;
  using vec_type = Tpetra::Vector<>;

  using scalar_type = multivec_type::scalar_type;
  using local_ordinal_type = multivec_type::local_ordinal_type;
  using global_ordinal_type = multivec_type::global_ordinal_type;

  // Read input parameters from command line
  Teuchos::CommandLineProcessor clp;
  Tpetra::global_size_t numGblIndices = 50; clp.setOption("n", &numGblIndices, "number of nodes / number of global indices (default: 50)");
  switch (clp.parse(argc, argv)) {
    case Teuchos::CommandLineProcessor::PARSE_HELP_PRINTED:        return EXIT_SUCCESS;
    case Teuchos::CommandLineProcessor::PARSE_ERROR:
    case Teuchos::CommandLineProcessor::PARSE_UNRECOGNIZED_OPTION: return EXIT_FAILURE;
    case Teuchos::CommandLineProcessor::PARSE_SUCCESSFUL:          break;
  }

  // Never create Tpetra objects at main() scope.
  // Never allow them to persist past ScopeGuard's destructor.
  Tpetra::ScopeGuard tpetraScope(&argc, &argv);
  {
    // Create MPI communicator via Tpetra and obtain local MPI rank and the
    // total size of the MPI communicator
    RCP<const Teuchos::Comm<int>> comm = Tpetra::getDefaultComm();
    const size_t myRank = comm->getRank();
    const size_t numProcs = comm->getSize();

    // Print the Trilinos version and the number of MPI ranks of the test
    RCP<Teuchos::FancyOStream> out = Teuchos::fancyOStream(Teuchos::rcpFromRef(std::cout));
    const bool verbose = (myRank == 0); // Only print on rank 0
    if (verbose) *out << Tpetra::version() << std::endl;
    if (verbose) *out << "Number of ranks: " << numProcs << std::endl;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    if (verbose) *out << "\n>> I. Create and print Tpetra map\n" << std::endl;

    // Construct a Map that puts approximately the same number of equations
    // on each processor; we start with index 0 (as standard in C++)
    /* START OF TODO: Create map */


    /* END OF TODO: Create map */

    // Print all information about the map (maximum verbosity: VERB_EXTREME)
    map->describe(*out, Teuchos::VERB_EXTREME);

    // Get the number of elements owned by the local MPI rank
    const size_t numMyElements = map->getLocalNumElements();

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    if (verbose) *out << "\n>> II. Create, fill, and print sparse matrix (Tpetra CrsMatrix)\n" << std::endl;

    // Create a Tpetra sparse matrix whose rows have distribution given by
    // the Map. We expect at most three entries per row.
    /* START OF TODO: Create empty matrix */

    /* END OF TODO: Create empty matrix */

    // We did not specify a column map => We will assemble using global
    // indices and let Trilinos create the column map for us when calling fillComplete().
    // Hence, we need to insert values using global indices (which is convenient but slow).

    // Fill the sparse matrix, one row at a time.
    const scalar_type two = static_cast<scalar_type>(2.0);
    const scalar_type negOne = static_cast<scalar_type>(-1.0);
    for (local_ordinal_type lclRow = 0; lclRow < static_cast<local_ordinal_type>(numMyElements); ++lclRow) {
      // Convert local index to global index
      /* START OF TODO: convert local to global index */

      /* END OF TODO: convert local to global index */

      // A(0, 0:1) = [2, -1]
      if (gblRow == 0) {
        /* START OF TODO: Fill first row */

        /* END OF TODO: Fill first row */
      }
      // A(N-1, N-2:N-1) = [-1, 2]
      else if (static_cast<Tpetra::global_size_t>(gblRow) == numGblIndices - 1) {
        /* START OF TODO: Fill last row */

        /* END OF TODO: Fill last row */
      }
      // A(i, i-1:i+1) = [-1, 2, -1]
      else {
        /* START OF TODO: Fill intermediate rows */


        /* END OF TODO: Fill intermediate rows */
      }
    }

    // Tell the sparse matrix that we are done adding entries to it. For
    // completeness, we specify the domain and range maps (here, both are
    // the map created before).
    /* START OF TODO: Fill complete */

    /* END OF TODO: Fill complete */

    // If matrix->fillComplete(); would be called instead (no arguments), the
    // same would happen since Trilinos would automatically use the row map
    // as domain and range ma
    // Print all information about the matrix (maximum verbosity:
    // VERB_EXTREME)
    A->describe(*out, Teuchos::VERB_EXTREME);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    if (verbose) *out << "\n>> III. Create right hand side and solution vectors and print the right hand side vector (Tpetra Vector)\n" << std::endl;

    // Solution vector
    RCP<vec_type> x(new vec_type(A->getDomainMap())); x->putScalar(Teuchos::ScalarTraits<scalar_type>::zero());

    // Right-hand side vector
    /* START OF TODO: Fill right-hand side */


    /* END OF TODO: Fill right-hand side */

    b->describe(*out, Teuchos::VERB_EXTREME);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    if (verbose) *out << "\n>> IV. Solve the system and print the right hand side (Tpetra Vector)\n" << std::endl;

    auto solver = Amesos2::create<crs_matrix_type, multivec_type>("Klu", A, x, b);
    solver->symbolicFactorization();
    solver->numericFactorization();
    solver->solve();

    x->describe(*out, Teuchos::VERB_EXTREME);

    return EXIT_SUCCESS;
  }
}
