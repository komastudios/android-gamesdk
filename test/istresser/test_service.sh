#!/usr/bin/env bash
uuid=$(uuidgen)
cp ../grabber/app/build/outputs/apk/debug/app-debug.apk ../grabber/app/build/outputs/apk/debug/grabber.apk
for i in `seq 1 26`;
    do
        gcloud beta firebase test android run --type=game-loop \
            --app=app/build/outputs/apk/debug/app-debug.apk \
            --additional-apks=../grabber/app/build/outputs/apk/debug/grabber.apk \
            --scenario-numbers=$i \
            --async \
            --timeout 15m \
            --results-history-name=$uuid \
            --device model=A1N_sprout,version=26 \
            --device model=ASUS_Z01H_1,version=26 \
            --device model=F8331,version=26 \
            --device model=F8332,version=26 \
            --device model=FRT,version=27 \
            --device model=G8142,version=26 \
            --device model=G8441,version=26 \
            --device model=H8216,version=28 \
            --device model=H8296,version=28 \
            --device model=H8324,version=26 \
            --device model=HWCOR,version=27 \
            --device model=OnePlus3T,version=26 \
            --device model=OnePlus5,version=26 \
            --device model=OnePlus5T,version=28 \
            --device model=OnePlus6T,version=28 \
            --device model=SOV33,version=26 \
            --device model=a9y18qlte,version=26 \
            --device model=addison,version=26 \
            --device model=albus,version=26 \
            --device model=aljeter_n,version=26 \
            --device model=astarqltechn,version=26 \
            --device model=astarqlteskt,version=26 \
            --device model=aura,version=27 \
            --device model=beyond1,version=28 \
            --device model=blueline,version=28 \
            --device model=blueline,version=29 \
            --device model=crownlte,version=28 \
            --device model=crownqlteue,version=27 \
            --device model=cruiserltesq,version=26 \
            --device model=deen_sprout,version=28 \
            --device model=dream2lte,version=26 \
            --device model=dream2qlteue,version=26 \
            --device model=dreamqlteue,version=26 \
            --device model=equuleus,version=28 \
            --device model=flame,version=29 \
            --device model=greatlte,version=28 \
            --device model=greatqlte,version=26 \
            --device model=hero2qltevzw,version=26 \
            --device model=heroqltetmo,version=26 \
            --device model=heroqltevzw,version=26 \
            --device model=htc_ocedugl,version=26 \
            --device model=htc_ocmdugl,version=26 \
            --device model=htc_ocndugl,version=26 \
            --device model=james,version=26 \
            --device model=jeter,version=26 \
            --device model=k61v1_basic_ref,version=29 \
            --device model=lithium,version=26 \
            --device model=nitrogen,version=28 \
            --device model=platina,version=28 \
            --device model=poseidonlteatt,version=26 \
            --device model=sailfish,version=26 \
            --device model=sailfish,version=27 \
            --device model=sailfish,version=28 \
            --device model=seoul,version=26 \
            --device model=star2lteks,version=28 \
            --device model=star2qltechn,version=26 \
            --device model=star2qlteue,version=26 \
            --device model=starlte,version=26 \
            --device model=starlteks,version=28 \
            --device model=starqltechn,version=26 \
            --device model=starqlteue,version=26 \
            --device model=taimen,version=26 \
            --device model=taimen,version=27 \
            --device model=tissot_sprout,version=28 \
            --device model=walleye,version=26 \
            --device model=walleye,version=27 \
            --device model=walleye,version=28
    done
