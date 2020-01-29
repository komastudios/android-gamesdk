#include <iostream>
#include "mesh_loader.cc"

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cout << "Please pass in only path to folder";
    return 0;
  }

  std::string fileName = argv[1];
  MeshLoader(fileName);
}
