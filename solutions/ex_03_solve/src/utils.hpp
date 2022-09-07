
#include <string>

#include <Galeri_XpetraProblemFactory.hpp>
#include <Galeri_XpetraMatrixTypes.hpp>
#include <Galeri_XpetraParameters.hpp>
#include <Galeri_XpetraUtils.hpp>
#include <Galeri_XpetraMaps.hpp>

#include <Teuchos_Comm.hpp>
#include <Teuchos_RCP.hpp>

#include <Tpetra_CrsMatrix.hpp>
#include <Tpetra_Vector.hpp>

#include <Xpetra_CrsMatrix.hpp>
#include <Xpetra_CrsMatrixWrap.hpp>
#include <Xpetra_Map.hpp>
#include <Xpetra_MapFactory.hpp>
#include <Xpetra_Matrix.hpp>
#include <Xpetra_MultiVector.hpp>
#include <Xpetra_TpetraCrsMatrix.hpp>

using Scalar = Tpetra::CrsMatrix<>::scalar_type;
using LocalOrdinal = Tpetra::CrsMatrix<>::local_ordinal_type;
using GlobalOrdinal = Tpetra::CrsMatrix<>::global_ordinal_type;
using Node = Tpetra::CrsMatrix<>::node_type;

using Teuchos::RCP;
using Teuchos::rcp;

RCP<const Tpetra::CrsMatrix<Scalar,LocalOrdinal,GlobalOrdinal,Node>>
buildMatrix(Teuchos::ParameterList& galeriList, RCP<const Teuchos::Comm<int>> comm)
{
  using XTeptraCrsMatrix = Xpetra::TpetraCrsMatrix<Scalar,LocalOrdinal,GlobalOrdinal,Node>;
  using XMap = Xpetra::Map<LocalOrdinal,GlobalOrdinal,Node>;
  using XMapFactory = Xpetra::MapFactory<LocalOrdinal,GlobalOrdinal,Node>;
  using XMultiVector = Xpetra::MultiVector<Scalar,LocalOrdinal,GlobalOrdinal,Node>;

  using crs_matrix_type = Tpetra::CrsMatrix<>;

  Xpetra::UnderlyingLib lib = Xpetra::UseTpetra ;

  const GlobalOrdinal nx = galeriList.get<GlobalOrdinal>("nx");
  const GlobalOrdinal ny = galeriList.get<GlobalOrdinal>("ny");
  const GlobalOrdinal nz = galeriList.get<GlobalOrdinal>("nz");

  const std::string matrixType = galeriList.get<std::string>("matrixType");

  // Create the node map
  std::string gridType = "";
  if (matrixType == "Laplace1D") {
    gridType = "Cartesian1D";
  } else if (matrixType == "Laplace2D" || matrixType == "Star2D" ||
             matrixType == "BigStar2D" || matrixType == "Elasticity2D") {
    gridType = "Cartesian2D";
  } else if (matrixType == "Laplace3D" || matrixType == "Brick3D" || matrixType == "Elasticity3D") {
    gridType = "Cartesian3D";
  }
  RCP<const XMap> nodeMap = Galeri::Xpetra::CreateMap<LocalOrdinal,GlobalOrdinal,Node>(lib, gridType, comm, galeriList);

  // Expand map to do multiple DOF per node for block problems
  int numDofsPerNode = 0;
  if (matrixType == "Elasticity2D") {
    numDofsPerNode = 2;
  } else if (matrixType == "Elasticity3D") {
    numDofsPerNode = 3;
  } else {
    numDofsPerNode = 1;
  }
  RCP<const XMap> dofMap = Xpetra::MapFactory<LocalOrdinal,GlobalOrdinal,Node>::Build(nodeMap, numDofsPerNode);

  // Set meaningful boundary conditions in case of elasticity problems
  if (matrixType == "Elasticity2D" || matrixType == "Elasticity3D") {
    // Our default test case for elasticity: all boundaries of a square/cube have Neumann b.c. except left which has Dirichlet
    galeriList.set("right boundary" , "Neumann");
    galeriList.set("bottom boundary", "Neumann");
    galeriList.set("top boundary"   , "Neumann");
    galeriList.set("front boundary" , "Neumann");
    galeriList.set("back boundary"  , "Neumann");
  }

  RCP<Galeri::Xpetra::Problem<XMap,XTeptraCrsMatrix,XMultiVector> > problem =
    Galeri::Xpetra::BuildProblem<Scalar,LocalOrdinal,GlobalOrdinal,XMap,XTeptraCrsMatrix,XMultiVector>(matrixType, dofMap, galeriList);
  RCP<XTeptraCrsMatrix> tpetraCrsMatrix = problem->BuildMatrix();

  return tpetraCrsMatrix->getTpetra_CrsMatrix();
}

void createLinearSystem(Teuchos::ParameterList& galeriList, RCP<const Teuchos::Comm<int>> comm,
    RCP<const Tpetra::CrsMatrix<Scalar,LocalOrdinal,GlobalOrdinal,Node>>& A,
    RCP<Tpetra::Vector<Scalar,LocalOrdinal,GlobalOrdinal,Node>>& x,
    RCP<Tpetra::Vector<Scalar,LocalOrdinal,GlobalOrdinal,Node>>& b)
{
  using Vector = Tpetra::Vector<Scalar,LocalOrdinal,GlobalOrdinal,Node>;

  A = buildMatrix(galeriList, comm);
  x = rcp(new Vector(A->getDomainMap(), true));
  b = rcp(new Vector(A->getRangeMap(),true));

  x->randomize();
  A->apply(*x, *b);
  x->putScalar(Teuchos::ScalarTraits<Scalar>::zero());
}
