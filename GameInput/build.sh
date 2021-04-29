set -e

../gradlew :GameInput:assembleRelease

# Inject Prefab in the AAR. This is a temporary solution while we find a way
# to make prefabPublishing to work in build.gradle
rm -rf tmp-aar/
unzip ../../out/outputs/aar/GameInput-release.aar -d tmp-aar/
cp -r prefab-src tmp-aar/
mv tmp-aar/prefab-src tmp-aar/prefab
rm -rf tmp-aar/prefab/modules/game-input/libs/.DS_Store # Ensure the folder is empty
pushd tmp-aar/
rm -rf ../../../out/outputs/aar/GameInput-release.aar
zip ../../../out/outputs/aar/GameInput-release.aar -r *
popd
rm -rf tmp-aar/
zipinfo ../../out/outputs/aar/GameInput-release.aar