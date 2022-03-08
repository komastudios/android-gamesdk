/*
 * Copyright 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "anim.hpp"
#include "game_asset_manager.hpp"
#include "game_asset_manifest.hpp"
#include "loader_scene.hpp"
#include "tuning_manager.hpp"
#include "welcome_scene.hpp"
#include "strings.inl"

#define TEXT_COLOR 0.0f, 1.0f, 0.0f
#define TEXT_POS center, 0.60f
#define TEXT_FONT_SCALE 1.0f

#define LOADING_TIMEOUT 5000

#define MAX_ASSET_TEXTURES 16

LoaderScene *currentScene = NULL;

class LoaderScene::TextureLoader {
 private:
    int _totalLoadCount = 0;
    int _currentLoadIndex = 0;
    int _remainingLoadCount = 0;

    struct LoadedTextureData {
        LoadedTextureData() : textureSize(0), textureData(NULL), textureName(NULL) {}

        size_t textureSize;
        void *textureData;
        const char *textureName;
    };

    LoadedTextureData _loadedTextures[MAX_ASSET_TEXTURES];

 public:
    ~TextureLoader() {
        // Wait for any textures to finish loading so we don't accidentally call
        // callbacks on a deleted loader.
        constexpr int MAX_WAITS = 20;
        int waits = MAX_WAITS;
        while(_remainingLoadCount != 0 && waits > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            waits--;
        }
        if (waits == 0) {
            ALOGE("Timed-out waiting for textures to load");
            exit(1);
        }
    }

    int TotalNumberToLoad() const { return _totalLoadCount; }
    int NumberCompetedLoading() const { return _totalLoadCount - _remainingLoadCount; }
    int NumberRemainingToLoad() const { return _remainingLoadCount; }

    void LoadingCallback(const LoadingCompleteMessage *message) {
        if (message->loadSuccessful) {
            if (_currentLoadIndex < MAX_ASSET_TEXTURES) {
                _loadedTextures[_currentLoadIndex].textureSize = message->bytesRead;
                _loadedTextures[_currentLoadIndex].textureData = message->loadBuffer;
                _loadedTextures[_currentLoadIndex].textureName = message->assetName;
                ++_currentLoadIndex;
            }
            ALOGI("Finished async load %s", message->assetName);
        } else {
            ALOGE("Async load failed for %s", message->assetName);
        }
        --_remainingLoadCount;
    }

    static void LoadingCallbackProxy(const LoadingCompleteMessage *message) {
        TextureLoader* loader = (TextureLoader*)message->userData;
        loader->LoadingCallback(message);
    }

    void LoadTexturesFromAssetPack(const char *assetPackName) {
        GameAssetManager *gameAssetManager = NativeEngine::GetInstance()->GetGameAssetManager();
        int assetPackFileCount = 0;
        const char **assetPackFiles = gameAssetManager->GetGameAssetPackFileList(assetPackName,
                &assetPackFileCount);
        if (assetPackFiles != NULL) {
            for (int i = 0; i < assetPackFileCount; ++i) {
                uint64_t fileSize = gameAssetManager->GetGameAssetSize(assetPackFiles[i]);
                if (fileSize > 0) {
                    uint8_t *fileBuffer = static_cast<uint8_t *>(malloc(fileSize));
                    if (gameAssetManager->LoadGameAssetAsync(assetPackFiles[i], fileSize,
                                                             fileBuffer, LoadingCallbackProxy,
                                                             this)) {
                        ++_totalLoadCount;
                        ++_remainingLoadCount;
                        ALOGI("Started async load %s", assetPackFiles[i]);
                    }
                }
            }
        }
    }

    void CreateTextures() {
        TextureManager *textureManager = NativeEngine::GetInstance()->GetTextureManager();
        for (int i = 0; i < _currentLoadIndex; ++i) {
            textureManager->CreateTexture(_loadedTextures[i].textureName,
                _loadedTextures[i].textureSize,
                static_cast<const uint8_t *>(_loadedTextures[i].textureData));
        }
    }
}; // class LoaderScene::TextureLoader

LoaderScene::LoaderScene() : mTextureLoader(new LoaderScene::TextureLoader()) {
    mLoadingText = NULL;
    mLoadingWidget = NULL;
    mTextBoxId = -1;
    mStartTime = 0;
    currentScene = this;
    const char *savePath = NativeEngine::GetInstance()->GetInternalStoragePath();
    int len = strlen(savePath) + strlen(SAVE_FILE_NAME) + 3;
    mSaveFileName = new char[len];
    strcpy(mSaveFileName, savePath);
    strcat(mSaveFileName, "/");
    strcat(mSaveFileName, SAVE_FILE_NAME);

    mLevelLoaded = LoadLocalProgress();
    mUseCloudSave = NativeEngine::GetInstance()->IsCloudSaveEnabled();
    if (mUseCloudSave) {
        mUserAuthenticationCompleted = false;
        mCloudDataLoadingCompleted = false;
        ALOGI("Cloud save detected. Attempting to load cloud data.");
        NativeEngine::GetInstance()->LoadCloudData();
    } else {
        ALOGI("Cloud save not detected, using local storage: %d", mLevelLoaded);
        mUserAuthenticationCompleted = true;
        mCloudDataLoadingCompleted = true;
    }
}

LoaderScene::~LoaderScene() {
    currentScene = NULL;
}

void LoaderScene::DoFrame() {
    if (mTextureLoader->NumberRemainingToLoad() == 0 && mCloudDataLoadingCompleted) {
        mTextureLoader->CreateTextures();

        // Inform performance tuner we are done loading
        TuningManager *tuningManager = NativeEngine::GetInstance()->GetTuningManager();
        tuningManager->FinishLoading();

        timespec currentTimeSpec;
        clock_gettime(CLOCK_MONOTONIC, &currentTimeSpec);
        uint64_t currentTime = currentTimeSpec.tv_sec * 1000 + (currentTimeSpec.tv_nsec / 1000000);
        uint64_t deltaTime = currentTime - mStartTime;
        float loadTime = deltaTime;
        loadTime /= 1000.0f;
        ALOGI("Load complete in %.1f seconds", loadTime);
        SceneManager *mgr = SceneManager::GetInstance();
        mgr->RequestNewScene(new WelcomeScene(mLevelLoaded));
    } else {
        float totalLoad = mTextureLoader->TotalNumberToLoad() + LOAD_DATA_STEPS;
        float completedLoad = mTextureLoader->NumberCompetedLoading();
        // Check load data steps completed
        if (mUserAuthenticationCompleted) {
            completedLoad += 1.0;
        }
        if (mCloudDataLoadingCompleted) {
            completedLoad += 1.0;
        }

        int loadingPercentage = static_cast<int>((completedLoad / totalLoad) * 100.0f);
        char progressString[64];
        sprintf(progressString, "%s... %d%%", S_LOADING, loadingPercentage);
        mLoadingWidget->SetText(progressString);
    }

    UiScene::DoFrame();
}

void LoaderScene::OnCreateWidgets() {
    float maxX = SceneManager::GetInstance()->GetScreenAspect();
    float center = 0.5f * maxX;

    mLoadingWidget = NewWidget()->SetText(S_LOADING)->SetTextColor(TEXT_COLOR)
            ->SetCenter(TEXT_POS);
    mTextBoxId = mLoadingWidget->GetId();

    timespec currentTimeSpec;
    clock_gettime(CLOCK_MONOTONIC, &currentTimeSpec);
    mStartTime = currentTimeSpec.tv_sec * 1000 + (currentTimeSpec.tv_nsec / 1000000);
    mTextureLoader->LoadTexturesFromAssetPack(GameAssetManifest::MAIN_ASSETPACK_NAME);
    GameAssetManager *gameAssetManager = NativeEngine::GetInstance()->GetGameAssetManager();
    if (gameAssetManager->GetGameAssetPackStatus(GameAssetManifest::EXPANSION_ASSETPACK_NAME) ==
        GameAssetManager::GAMEASSET_READY) {
        mTextureLoader->LoadTexturesFromAssetPack(GameAssetManifest::EXPANSION_ASSETPACK_NAME);
    }
}

void LoaderScene::RenderBackground() {
    RenderBackgroundAnimation(mShapeRenderer);
}

int LoaderScene::LoadLocalProgress() {
    ALOGI("Attempting to load locally: %s", mSaveFileName);
    int level = 0;
    FILE *f = fopen(mSaveFileName, "r");
    if (f) {
        ALOGI("Local file found. Loading data.");
        if (1 != fscanf(f, "v1 %d", &level)) {
            ALOGE("Error parsing save file.");
            level = 0;
        } else {
            ALOGI("Loaded. Level = %d", level);
        }
        fclose(f);
    } else {
        ALOGI("Save file not present.");
    }
    return level;
}

void LoaderScene::CloudLoadUpdate(int result, int level) {
    if (result == AUTHENTICATION_COMPLETED) {
        ALOGI("Cloud authentication completed");
        mUserAuthenticationCompleted = true;
    } else if (result == AUTHENTICATION_ERROR) {
        ALOGE("Error authenticating the user. Using local data instead.");
        mCloudDataLoadingCompleted = true;
    } else if (result == LOAD_DATA_COMPLETED) {
        ALOGI("Using cloud save data. Level loaded: %d", level);
        // Assert previous step was completed
        MY_ASSERT(mUserAuthenticationCompleted);
        mLevelLoaded = level;
        mCloudDataLoadingCompleted = true;
    } else if (result == ERROR_LOADING_DATA) {
        ALOGE("Error on loading cloud data. Using local data instead.");
        mCloudDataLoadingCompleted = true;
    }
}

LoaderScene * LoaderScene::GetCurrentScene() {
    MY_ASSERT(currentScene != NULL);
    return currentScene;
}

// TODO: rename the method according to your package name
extern "C" void Java_com_google_sample_agdktunnel_PGSManager_cloudLoadUpdate(
        JNIEnv *env, jobject thiz, jint result, jint level) {
    ALOGI("Update callback received: %d %d", (int)result, (int)level);
    LoaderScene *scene = LoaderScene::GetCurrentScene();
    scene->CloudLoadUpdate(result, level);
}