#include <iostream>

#include <Teuchos_Comm.hpp>
#include <Teuchos_DefaultComm.hpp>
#include <Teuchos_RCP.hpp>

#include <Tpetra_Core.hpp>
#include <Tpetra_Map.hpp>

int main(int argc, char *argv[]) {
  using Teuchos::RCP;
  using Teuchos::rcp;

  Tpetra::ScopeGuard tpetraScope(&argc, &argv);

  RCP<const Teuchos::Comm<int>> comm = Teuchos::DefaultComm<int>::getComm();
  RCP<Teuchos::FancyOStream> out =
      Teuchos::fancyOStream(Teuchos::rcpFromRef(std::cout));
  *out << "Number of ranks: " << comm->getSize() << std::endl;

  return 0;
}
