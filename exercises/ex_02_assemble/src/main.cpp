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

  typedef Tpetra::MultiVector<> multivec_type;
  typedef multivec_type::scalar_type scalar_type;
  typedef multivec_type::local_ordinal_type local_ordinal_type;
  typedef multivec_type::global_ordinal_type global_ordinal_type;
  typedef multivec_type::mag_type magnitude_type;
  typedef Tpetra::Map<local_ordinal_type,global_ordinal_type> map_type;
  typedef Tpetra::Vector<scalar_type,local_ordinal_type,global_ordinal_type> vec_type;
  typedef Tpetra::CrsMatrix<scalar_type,local_ordinal_type,global_ordinal_type> crs_matrix_type;

  typedef typename crs_matrix_type::nonconst_global_inds_host_view_type gids_type;
  typedef typename crs_matrix_type::nonconst_values_host_view_type vals_type;

  // Read input parameters from command line
  Teuchos::CommandLineProcessor clp;
  Tpetra::global_size_t numGblIndices = 50; clp.setOption("n", &numGblIndices, "number of nodes / number of global indices (default: 50)");

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

    // Only print on rank 0
    const bool verbose = (myRank == 0);
    if (verbose) *out << Tpetra::version() << std::endl;
    if (verbose) *out << "Number of ranks: " << numProcs << std::endl;

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    if (verbose) *out << std::endl;
    if (verbose) *out << ">> I. Create and print Tpetra map\n";
    if (verbose) *out << std::endl;

    // Construct a Map that puts approximately the same number of equations
    // on each processor; we start with index 0 (as standard in C++)
    const global_ordinal_type indexBase = 0;
    RCP<const map_type> map = rcp(new map_type(numGblIndices, indexBase, comm));

    // Print all information about the map (maximum verbosity: VERB_EXTREME)
    map->describe(*out, Teuchos::VERB_EXTREME);

    // Get the number of elements owned by the local MPI rank
    const size_t numMyElements = map->getLocalNumElements();

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    if (verbose) *out << std::endl;
    if (verbose) *out << ">> II. Create, fill, and print sparse matrix (Tpetra CrsMatrix)\n";
    if (verbose) *out << std::endl;

    // Create a Tpetra sparse matrix whose rows have distribution given by
    // the Map. We expect at most three entries per row.
    RCP<crs_matrix_type> A(new crs_matrix_type (map, 3));

    // We did not specify a column map => We will assemble using global
    // indices and let Trilinos create the column map for us when calling
    // fillComplete(
    // Fill the sparse matrix, one row at a time.
    const scalar_type two = static_cast<scalar_type>(2.0);
    const scalar_type negOne = static_cast<scalar_type>(-1.0);
    for (local_ordinal_type lclRow = 0; lclRow < static_cast<local_ordinal_type>(numMyElements); ++lclRow) {
      const global_ordinal_type gblRow = map->getGlobalElement (lclRow);
      // A(0, 0:1) = [2, -1]
      if (gblRow == 0) {
        A->insertGlobalValues(gblRow,
          tuple<global_ordinal_type>(gblRow, gblRow+1),
          tuple<scalar_type>(two, negOne));
      }
      // A(N-1, N-2:N-1) = [-1, 2]
      else if (static_cast<Tpetra::global_size_t>(gblRow) == numGblIndices-1) {
        A->insertGlobalValues(gblRow,
          tuple<global_ordinal_type>(gblRow-1, gblRow),
          tuple<scalar_type>(negOne, two));
      }
      // A(i, i-1:i+1) = [-1, 2, -1]
      else {
        A->insertGlobalValues(gblRow,
          tuple<global_ordinal_type>(gblRow-1, gblRow, gblRow+1),
          tuple<scalar_type>(negOne, two, negOne));
      }
    }

    // Tell the sparse matrix that we are done adding entries to it. For
    // completeness, we specify the domain and range maps (here, both are
    // the map created before).
    A->fillComplete(map, map);

    // If matrix->fillComplete(); would be called instead (no arguments), the
    // same would happen since Trilinos would automatically use the row map
    // as domain and range ma
    // Print all information about the matrix (maximum verbosity:
    // VERB_EXTREME)
    A->describe(*out, Teuchos::VERB_EXTREME);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    if (verbose) *out << std::endl;
    if (verbose) *out << ">> III. Create right hand side and solution vectors and print the right hand side vector (Tpetra Vector)\n";
    if (verbose) *out << std::endl;

    RCP<vec_type> b(new vec_type(A->getRangeMap())); b->putScalar(Teuchos::ScalarTraits<scalar_type>::one());
    RCP<vec_type> x(new vec_type(A->getDomainMap())); x->putScalar(Teuchos::ScalarTraits<scalar_type>::zero());

    b->describe(*out, Teuchos::VERB_EXTREME);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    if (verbose) *out << std::endl;
    if (verbose) *out << ">> IV. Solve the system and print the right hand side (Tpetra Vector)\n";
    if (verbose) *out << std::endl;

    auto solver = Amesos2::create<crs_matrix_type,multivec_type>("Klu", A, x, b);
    solver->symbolicFactorization();
    solver->numericFactorization();
    solver->solve();

    x->describe(*out, Teuchos::VERB_EXTREME);

    return 0;
  }
}
