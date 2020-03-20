#!/bin/bash

convert_pngs_recursive () {
  for file in $1/* ; do
    if [[ -d $file ]]; then
      convert_pngs_recursive $file
    else
      fname="${file%.*}"
      ext="${file##*.}"
      if [[ $ext == "png" ]]; then
        magick convert -flip "./"$file "./"$fname"-temp.png"
        ./astcenc-nointrin -cl "./"$fname"-temp.png" "./"$fname".astc" 4x4 -thorough
        rm "./"$fname"-temp.png"
      fi
      fi
  done
}

convert_pngs_recursive app/src/main/assets/textures/textures
