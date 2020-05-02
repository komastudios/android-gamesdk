#!/bin/sh

git clone --branch "0.9.7.0" https://github.com/g-truc/glm.git AndroidCertTest/common/3rd_party/glm
git clone --branch "v3.7.0" https://github.com/nlohmann/json.git AndroidCertTest/common/3rd_party/json
git clone https://github.com/BinomialLLC/basis_universal.git AndroidCertTest/common/3rd_party/basis_universal
cd AndroidCertTest/common/3rd_party/basis_universal 
git checkout 374a04d54dd8fa5218ecc03f2cd4834dfff15db6
cd ../../../..
