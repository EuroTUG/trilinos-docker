/* This example demonstrate the solution of a linear system with a preconditioner Krylov solver
 * with the help of the packages Belos and Ifpack2.
 */

#include "utils.hpp"

#include <cstdlib>

#include <BelosBlockCGSolMgr.hpp>
#include <BelosConfigDefs.hpp>
#include <BelosLinearProblem.hpp>
#include <BelosTpetraAdapter.hpp>
#include <BelosTypes.hpp>

#include <Ifpack2_Factory.hpp>
#include <Ifpack2_Preconditioner.hpp>

#include <Teuchos_ParameterList.hpp>
#include <Teuchos_RCP.hpp>

#include <Tpetra_Core.hpp>
#include <Tpetra_CrsMatrix.hpp>
#include <Tpetra_MultiVector.hpp>
#include <Tpetra_Operator.hpp>
#include <Tpetra_Vector.hpp>

int main(int argc, char *argv[]) {
  using Teuchos::ParameterList;
  using Teuchos::RCP;
  using Teuchos::rcp;
  using Teuchos::tuple;

  using scalar_type = Tpetra::MultiVector<>::scalar_type;
  using local_ordinal_type = Tpetra::MultiVector<>::local_ordinal_type;
  using global_ordinal_type = Tpetra::MultiVector<>::global_ordinal_type;
  using node_type = Tpetra::MultiVector<>::node_type;

  using crs_matrix_type = Tpetra::CrsMatrix<scalar_type, local_ordinal_type, global_ordinal_type, node_type>;
  using multivec_type = Tpetra::MultiVector<scalar_type, local_ordinal_type, global_ordinal_type, node_type>;
  using operator_type = Tpetra::Operator<scalar_type, local_ordinal_type, global_ordinal_type, node_type>;
  using row_matrix_type = Tpetra::RowMatrix<>;
  using vec_type = Tpetra::Vector<scalar_type, local_ordinal_type, global_ordinal_type, node_type>;

  using prec_type = Ifpack2::Preconditioner<>;
  using problem_type = Belos::LinearProblem<scalar_type, multivec_type, operator_type>;
  using solver_type = Belos::SolverManager<scalar_type, multivec_type, operator_type>;

  // Read input parameters from command line
  Teuchos::CommandLineProcessor clp;
  std::string matrixType = "Laplace2D"; clp.setOption("matrixType", &matrixType, "Type of problem to be solved [Laplace1D, Laplace2D, Laplace3D, Elasticity2D, Elasticity3D] (default: Laplace2D)");
  global_ordinal_type nx = 10; clp.setOption("nx", &nx, "Number of mesh nodes in x-direction");
  global_ordinal_type ny = 10; clp.setOption("ny", &ny, "Number of mesh nodes in y-direction");
  global_ordinal_type nz = 10; clp.setOption("nz", &nz, "Number of mesh nodes in z-direction");

  scalar_type tol = 1.0e-4; clp.setOption("tol", &tol, "Tolerance to check for convergence of Krylov solver");
  int maxIters = 100; clp.setOption("maxIters", &maxIters, "Maximum number of iterations of the Krylov solver");
  bool usePreconditioner = false; clp.setOption("withPreconditioner", "noPreconditioner", &usePreconditioner, "Flag to activate/deactivate the preconditioner.");

  std::string relaxationType = "Jacobi"; clp.setOption("precType", &relaxationType, "Type of preconditioner [Jacobi, Gauss-Seidel, Symmetric Gauss-Seidel] (default: Jacobi)");
  int numSweeps = 1; clp.setOption("numSweeps", &numSweeps, "Number of relaxation sweeps in the preconditioner (default: 1)");
  double damping = 2./3.; clp.setOption("damping", &damping, "Damping parameter for relaxation preconditioner (default: 2/3)");

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
    RCP<Teuchos::FancyOStream> out = Teuchos::fancyOStream(Teuchos::rcpFromRef(std::cout));
    out->setOutputToRootOnly(0);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    *out << ">> I. Create linear system A*x=b for a " << matrixType << " problem." << std::endl;

    ParameterList galeriList;
    galeriList.set("nx", nx);
    galeriList.set("ny", ny);
    galeriList.set("nz", nz);
    galeriList.set("matrixType", matrixType);
    RCP<const crs_matrix_type> matrix = Teuchos::null;
    RCP<vec_type> x = Teuchos::null;
    RCP<vec_type> rhs = Teuchos::null;
    createLinearSystem(galeriList, comm, matrix, x, rhs);

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    *out << ">> II. Create a ";
    if (usePreconditioner) *out << "preconditioned ";
    *out << "GMRES solver from the Belos package." << std::endl;

    // Create Belos iterative linear solver
    RCP<solver_type> solver = Teuchos::null;
    RCP<ParameterList> solverParams = rcp (new ParameterList());
    {
      int verbLevel = Belos::Errors + Belos::Warnings + Belos::FinalSummary;
      solverParams->set( "Verbosity", verbLevel );

      solverParams->set("Maximum Iterations", maxIters);
      solverParams->set("Convergence Tolerance", tol);

      /* START OF TODO: Create Belos solver */
      Belos::SolverFactory<scalar_type, multivec_type, operator_type> belosFactory;
      solver = belosFactory.create ("GMRES", solverParams);
      /* END OF TODO: Create Belos solver */
    }
    if (solver.is_null ()) {
      if (comm->getRank () == 0) {
        cerr << "Failed to create Belos solver!" << endl;
      }
      return EXIT_FAILURE;
    }

    // Optionally, create Ifpack2 preconditioner.
    RCP<prec_type> prec = Teuchos::null;
    if (usePreconditioner)
    {
      /* START OF TODO: Create preconditioner */
      prec = Ifpack2::Factory::create<row_matrix_type> ("RELAXATION", matrix);
      /* END OF TODO: Create preconditioner */
      if (prec.is_null ()) {
        *out << "Failed to create Ifpack2 preconditioner!" << std::endl;
        return EXIT_FAILURE;
      }

      // Pass parameters to the preconditioner
      /* START OF TODO: Configure preconditioner */
      ParameterList precParams;
      precParams.set("relaxation: type", relaxationType);
      precParams.set("relaxation: sweeps", numSweeps);
      precParams.set("relaxation: damping factor", damping);
      prec->setParameters(precParams);
      /* END OF TODO: Configure preconditioner */

      // Setup the preconditioner
      /* START OF TODO: Setup the preconditioner */
      prec->initialize ();
      prec->compute ();
      /* END OF TODO: Setup the preconditioner */
    }

    // Set up the linear problem to solve.
    RCP<problem_type> problem = Teuchos::null;
    {
      /* START OF TODO: Define linear problem */
      problem = rcp(new problem_type (matrix, x, rhs));
      /* END OF TODO: Define linear problem */

      if (!prec.is_null()) {
        /* START OF TODO: Insert preconditioner */
        problem->setRightPrec(prec);
        /* END OF TODO: Insert preconditioner */
      }

      /* START OF TODO: Set the linear problem */
      problem->setProblem();
      solver->setProblem(problem);
      /* END OF TODO: Set the linear problem */
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    *out << ">> III. Solve the linear system." << std::endl;

    // Solve the linear system.
    {
      /* START OF TODO: Solve */
      Belos::ReturnType solveResult = solver->solve();
      /* END OF TODO: Solve */
      if (solveResult == Belos::Unconverged)
      {
        *out << "Belos did not converge in " << solver->getNumIters() << " iterations." << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        *out << "Belos converged in " << solver->getNumIters()
            << " iterations to an achieved tolerance of " << solver->achievedTol()
            << " (< tol = " << tol << ")." << std::endl;
      }
    }

    return EXIT_SUCCESS;
  }
}
