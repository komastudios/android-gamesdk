#include <iostream>
#include "mesh_loader.cc"
#include "../../../../../external/filesystem.hpp"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Please pass in only path to input folder";
    return 0;
  }

  std::string inputFolderName = argv[1];

  for (auto &file_path_ : ghc::filesystem::directory_iterator(inputFolderName)) {
    if (file_path_.path().extension() == ".obj") {
      std::string file_name_ = file_path_.path().filename().string();
      file_name_ = file_name_.substr(0, file_name_.find('.'));
      MeshLoader(file_path_.path().string(), file_name_);
    }
  }
}
