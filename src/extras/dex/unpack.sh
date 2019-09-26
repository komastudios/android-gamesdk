# $1 = directory of dex compiler, 'dx'
# $2 = path of aar file
export PATH=$1:$PATH
jar xf $2
jar xf classes.jar 

clang -o bin2c bin2c_src/bin2c.c

dx --dex --output ChoreographerCallback.dex com/google/androidgamesdk/ChoreographerCallback*
./bin2c ChoreographerCallback.dex ChoreographerCallback_dex.c ChoreographerCallback_bytes

#dx --dex --output GameSdkDeviceInfoJni.dex com/google/androidgamesdk/GameSdkDeviceInfoJni*
#./bin2c GameSdkDeviceInfoJni.dex GameSdkDeviceInfoJni_dex.c GameSdkDeviceInfoJni_bytes

dx --dex --output SwappyDisplayManager.dex com/google/androidgamesdk/SwappyDisplayManager*
./bin2c SwappyDisplayManager.dex SwappyDisplayManager_dex.c SwappyDisplayManager_bytes

cp -f *_dex.c ../../../src/swappy/common
