set -e

../gradlew :GameActivity:assembleRelease

# Inject Prefab in the AAR. This is a temporary solution while we find a way
# to make prefabPublishing to work in build.gradle
rm -rf tmp-aar/
unzip ../../out/outputs/aar/GameActivity.aar -d tmp-aar/
cp -r prefab-src tmp-aar/
mv tmp-aar/prefab-src tmp-aar/prefab
rm -rf tmp-aar/prefab/modules/game-activity/libs/.DS_Store # Ensure the folder is empty
pushd tmp-aar/
rm -rf ../../../out/outputs/aar/GameActivity.aar
zip ../../../out/outputs/aar/GameActivity.aar -r *
popd
rm -rf tmp-aar/
zipinfo ../../out/outputs/aar/GameActivity.aar