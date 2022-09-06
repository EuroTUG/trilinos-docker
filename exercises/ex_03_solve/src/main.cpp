/* This example demonstrate the solution of a linear system with a preconditioner Krylov solver
 * with the help of the packages Belos and Ifpack2.
 */

#include "utils.hpp"

#include <BelosBlockCGSolMgr.hpp>
// #include <BelosBlockGmresSolMgr.hpp>
#include <BelosConfigDefs.hpp>
#include <BelosLinearProblem.hpp>
#include <BelosMultiVecTraits.hpp>
#include <BelosOperatorTraits.hpp>
// #include <BelosSolverFactory.hpp>
// #include <BelosSolverManager.hpp>
#include <BelosTpetraAdapter.hpp>
#include <BelosTypes.hpp>

#include <Teuchos_Array.hpp>
#include <Teuchos_RCP.hpp>
#include <Teuchos_ScalarTraits.hpp>

#include <Tpetra_Core.hpp>
#include <Tpetra_CrsMatrix.hpp>
#include <Tpetra_Map.hpp>
#include <Tpetra_MultiVector.hpp>
#include <Tpetra_Operator.hpp>
#include <Tpetra_Vector.hpp>
#include <Tpetra_Version.hpp>

int main(int argc, char *argv[]) {
  using Teuchos::RCP;
  using Teuchos::rcp;
  using Teuchos::tuple;

  using scalar_type = Tpetra::MultiVector<>::scalar_type;
  using local_ordinal_type = Tpetra::MultiVector<>::local_ordinal_type;
  using global_ordinal_type = Tpetra::MultiVector<>::global_ordinal_type;
  using node_type = Tpetra::MultiVector<>::node_type;

  using crs_matrix_type = Tpetra::CrsMatrix<scalar_type, local_ordinal_type, global_ordinal_type, node_type>;
  using map_type = Tpetra::Map<local_ordinal_type, global_ordinal_type, node_type>;
  using multivec_type = Tpetra::MultiVector<scalar_type, local_ordinal_type, global_ordinal_type, node_type>;
  using operator_type = Tpetra::Operator<scalar_type, local_ordinal_type, global_ordinal_type, node_type>;
  using vec_type = Tpetra::Vector<scalar_type, local_ordinal_type, global_ordinal_type, node_type>;

  // using scalar_type = multivec_type::scalar_type;
  // using local_ordinal_type = multivec_type::local_ordinal_type;
  // using global_ordinal_type = multivec_type::global_ordinal_type;

  typedef Belos::OperatorTraits<scalar_type,multivec_type,operator_type> OPT;
  typedef Belos::MultiVecTraits<scalar_type,multivec_type> MVT;

  // Read input parameters from command line
  Teuchos::CommandLineProcessor clp;
  Tpetra::global_size_t numGblIndices = 50; clp.setOption("n", &numGblIndices, "number of nodes / number of global indices (default: 50)");
  std::string matrixType = "Laplace2D"; clp.setOption("matrixType", &matrixType, "Type of problem to be solved [Laplace1D, Laplace2D, Laplace3D, Elasticity2D, Elasticity3D] (default: Laplace2D)");
  scalar_type tol = 1.0e-4; clp.setOption("tol", &tol, "Tolerance to check for convergence of Krylov solver");
  int maxIters = -1; clp.setOption("maxIters", &maxIters, "Maximum number of iterations of the Krylov solver");

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

    // Create an output stream
    RCP<Teuchos::FancyOStream> allOut = Teuchos::fancyOStream(Teuchos::rcpFromRef(std::cout));
    RCP<Teuchos::FancyOStream> out = Teuchos::fancyOStream(Teuchos::rcpFromRef(std::cout));
    out->setOutputToRootOnly(0);

    // Define some useful scalar values
    const scalar_type zero = Teuchos::ScalarTraits<scalar_type>::zero();
    const scalar_type one = Teuchos::ScalarTraits<scalar_type>::one();

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    *out << "\n>> I. Create the system matrix for a " << matrixType << " problem.\n" << std::endl;

    Teuchos::ParameterList galeriList;
    int nx = 10;
    int ny = 10;
    int nz = 10;
    galeriList.set("nx", static_cast<global_ordinal_type>(nx));
    galeriList.set("ny", static_cast<global_ordinal_type>(ny));
    galeriList.set("nz", static_cast<global_ordinal_type>(nz));
    galeriList.set("matrixType", matrixType);
    RCP<const crs_matrix_type> matrix = buildMatrix(galeriList, comm);

    // matrix->describe(*allOut, Teuchos::VERB_EXTREME);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    *out << "\n>> II. Create and empty solution vector and a right-hand side vector will all ones.\n" << std::endl;

    RCP<vec_type> x = rcp(new vec_type(matrix->getDomainMap(), true));
    RCP<vec_type> rhs = rcp(new vec_type(matrix->getRangeMap(),true));
    MVT::MvRandom(*x);
    OPT::Apply(*matrix, *x, *rhs);
    MVT::MvInit(*x, zero);

    // RCP<vec_type> x = rcp(new vec_type(matrix->getDomainMap(), true));
    // RCP<vec_type> rhs = rcp(new vec_type(matrix->getRangeMap()));
    // rhs->putScalar(one);

    // x->describe(*allOut, Teuchos::VERB_EXTREME);
    // rhs->describe(*allOut, Teuchos::VERB_EXTREME);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    *out << "\n>> III. Create a CG solver from the Belos package.\n" << std::endl;

    const int numGlobalElements = rhs->getGlobalLength();
    if (maxIters == -1) {
      maxIters = numGlobalElements - 1; // maximum number of iterations to run
    }
    //
    Teuchos::ParameterList belosList;
    belosList.set( "Maximum Iterations", maxIters);       // Maximum number of iterations allowed
    belosList.set( "Convergence Tolerance", tol);         // Relative convergence tolerance requested
    belosList.set( "Num Blocks", 100);                    // Maximum number of blocks in Krylov subspace (max subspace size)
    belosList.set( "Flexible Gmres", false);              // Do not use FGMRES

    int verbLevel = Belos::Errors + Belos::Warnings;
    // if (debug) {
    //   verbLevel += Belos::Debug;
    // }
    // if (verbose) {
    //   verbLevel += Belos::TimingDetails + Belos::FinalSummary + Belos::StatusTestDetails;
    // }
    // belosList.set( "Verbosity", verbLevel );
    // if (verbose) {
    //   if (frequency > 0) {
    //     belosList.set( "Output Frequency", frequency );
    //   }
    // }
    // Construct an unpreconditioned linear problem instance.
    Belos::LinearProblem<scalar_type, multivec_type, operator_type> problem(matrix, x, rhs);
    problem.setInitResVec(rhs);
    bool set = problem.setProblem();
    if (set == false) {
      *out << std::endl << "ERROR:  Belos::LinearProblem failed to set up correctly!" << std::endl;
      return -1;
    }

    Belos::BlockCGSolMgr<scalar_type, multivec_type, operator_type> solver(Teuchos::rcpFromRef(problem), Teuchos::rcpFromRef(belosList));

    *out << "\nDimension of matrix: " << numGlobalElements
        << "\nMax number of Gmres iterations: " << maxIters
        << "\nRelative residual tolerance: " << tol
        << "\n" << std::endl;

    Belos::ReturnType ret = solver.solve();
    if (ret == Belos::Unconverged)
    {
      *out << "Belos did not converge in " << solver.getNumIters() << " iterations." << std::endl;
      return -1;
    }
    else
    {
      *out << "Belos converged in " << solver.getNumIters()
          << " iterations to an achieved tolerance of " << solver.achievedTol()
          << " (< tol = " << tol << ")." << std::endl;
    }

    return 0;
  }
}
