#!/usr/bin/env bash
uuid=$(uuidgen)
for i in `seq 1 13`;
    do
        gcloud firebase test android run --type=game-loop \
            --app=app/build/outputs/apk/debug/app-debug.apk \
            --scenario-numbers=$i \
            --async \
            --no-performance-metrics \
            --no-record-video \
            --timeout 15m \
            --results-history-name=$uuid \
            --device model=a5y17lte,version=24 \
            --device model=crownlte,version=28 \
            --device model=dream2lte,version=26 \
            --device model=greatlte,version=28 \
            --device model=hero2lte,version=23 \
            --device model=heroqltevzw,version=26 \
            --device model=starlte,version=26 \
            --device model=taimen,version=26 \
            --device model=taimen,version=27
    done
