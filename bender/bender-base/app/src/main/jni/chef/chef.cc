#include <iostream>
#include "mesh_loader.cc"
#include "filesystem.hpp"

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cout << "Please pass in only path to folder";
    return 0;
  }

  std::string inputFolderName = argv[1];

  for (auto &filePath : ghc::filesystem::directory_iterator(inputFolderName)){
    if (filePath.path().extension() == ".obj"){
      std::string fileName = filePath.path().filename().string();
      fileName = fileName.substr(0, fileName.find('.'));
      MeshLoader(filePath.path().string(), fileName);
    }
  }
}
