#include <iostream>
#include <filesystem>
#include "mesh_loader.cc"

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cout << "Please pass in only path to folder";
    return 0;
  }

  char *folder_path = argv[1];
  char *output_path = argv[2];

  const std::filesystem::path pathToShow = folder_path;
  auto iter = std::filesystem::recursive_directory_iterator(pathToShow);

  for (const auto &entry : iter)
  {
    const auto filenameStr = entry.path().filename().string();
    if (entry.is_directory())
    {
      std::cout << "dir:  " << filenameStr << '\n';
    }
    else if (entry.is_regular_file())
    {
      std::cout << "file: " << filenameStr << "\text: " << std::filesystem::path(filenameStr).extension() << "\n";
      if (std::filesystem::path(filenameStr).extension() == ".obj")
      {
        MeshLoader(std::string(filenameStr));
      }
    }
  }
}
