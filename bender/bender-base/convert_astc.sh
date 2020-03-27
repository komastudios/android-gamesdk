#!/bin/bash

convert_pngs_recursive () {
  for file in $1/* ; do
    if [[ -d $file ]]; then
      convert_pngs_recursive $file
    else
      fname="${file%.*}"
      ext="${file##*.}"
      if [[ $ext == "png" ]]; then
        magick convert -flip "./"$file "./"$fname"-0-temp.png"
        ./astcenc-sse2 -cl "./"$fname"-0-temp.png" "./"$fname".astc" 12x12 -fast

        i=1
        while true; do
          magick convert -filter Kaiser -resize 50% "./"$fname"-"$((i-1))"-temp.png" "./"$fname"-"$i"-temp.png"
          rm "./"$fname"-"$((i-1))"-temp.png"
          width=$(magick identify -ping -format '%w' "./"$fname"-"$i"-temp.png")
          height=$(magick identify -ping -format '%h' "./"$fname"-"$i"-temp.png")
          ./astcenc-sse2 -cl "./"$fname"-"$i"-temp.png" "./"$fname"-mip-"$i".astc" 12x12 -fast
          if [[ "$width" -le 1 || "$height" -le 1 ]]; then
            rm "./"$fname"-"$i"-temp.png"
            break
          fi
          ((i++))
        done
      fi
    fi
  done
}

convert_pngs_recursive app/src/main/assets/textures/textures
