
#include <string>

#include <Galeri_XpetraProblemFactory.hpp>
#include <Galeri_XpetraMatrixTypes.hpp>
#include <Galeri_XpetraParameters.hpp>
#include <Galeri_XpetraUtils.hpp>
#include <Galeri_XpetraMaps.hpp>

#include <Teuchos_Comm.hpp>
#include <Teuchos_RCP.hpp>

#include <Tpetra_CrsMatrix.hpp>

#include <Xpetra_CrsMatrixWrap.hpp>
#include <Xpetra_Map.hpp>
#include <Xpetra_MapFactory.hpp>
#include <Xpetra_MultiVector.hpp>
#include <Xpetra_TpetraCrsMatrix.hpp>

RCP<const Tpetra::CrsMatrix<Scalar,LocalOrdinal,GlobalOrdinal,Node>>
buildMatrix(Teuchos::ParameterList& matrixList, RCP<const Teuchos::Comm<int>> comm)
{
  using Teuchos::RCP;

  using XCrsMatrixWrap = Xpetra::CrsMatrixWrap<Scalar,LocalOrdinal,GlobalOrdinal,Node>;
  using XMap = Xpetra::Map<LocalOrdinal,GlobalOrdinal,Node>;
  using XMapFactory = Xpetra::MapFactory<LocalOrdinal,GlobalOrdinal,Node>;
  using XMultiVector = Xpetra::MultiVector<Scalar,LocalOrdinal,GlobalOrdinal,Node>;

  using crs_matrix_type = Tpetra::CrsMatrix<>;

  Xpetra::UnderlyingLib lib = Xpetra::Tpetra;

  const global_ordinal_type zero = Teuchos::ScalarTraits<GlobalOrdinal>::zero();

  GlobalOrdinal nx,ny,nz;
  nx = ny = nz = Teuchos::as<GlobalOrdinal>(5);
  nx = matrixList.get("nx", nx);
  ny = matrixList.get("ny", ny);
  nz = matrixList.get("nz", nz);

  const std::string matrixType = matrixList.get("matrixType", "Laplace2D");
  GlobalOrdinal numGlobalElements = zero;
  if (matrixType == "Laplace1D")
    numGlobalElements = nx;
  else if (matrixType == "Laplace2D" || matrixType == "Star2D" || matrixType == "Cross2D")
    numGlobalElements = nx * ny;
  else if(matrixType == "Elasticity2D")
    numGlobalElements = 2 * nx * ny;
  else if (matrixType == "Laplace3D" || matrixType == "Brick3D")
    numGlobalElements = nx * ny * nz;
  else if  (matrixType == "Elasticity3D")
    numGlobalElements = 3 * nx * ny * nz;
  else
    TEUCHOS_ASSERT(false, "Unkonwn matrix type" << matrixType.c_str());;

  RCP<const XMap> map = XMapFactory::Build(lib, numGlobalElements, 0, comm);
  RCP<Galeri::Xpetra::Problem<XMap,XCrsMatrixWrap,XMultiVector> > problem =
    Galeri::Xpetra::BuildProblem<Scalar,LocalOrdinal,GlobalOrdinal,XMap,XCrsMatrixWrap,XMultiVector>(matrixType, map, matrixList);
  RCP<XMatrix> xpetraMatrix = problem->BuildMatrix();

  return xpetraMatrix->getTpetra_CrsMatrix();
}
