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

#include "common.hpp"
#include "game_asset_manifest.hpp"

#define ELEMENTS_OF(x) (sizeof(x) / sizeof(x[0]))
#define MAX_ASSET_PATH_LENGTH 512

namespace {
    /*
     * This is a rudimentary 'asset manifest' embedded in the source code for simplicity.
     * A real world project might generate a manifest file as part of the asset pipeline.
     */

    const char *InstallFileList[] = {"textures/wall1.tex", "textures/wall2.tex"};
    const char *OnDemandFileList[] = {"textures/wall3.tex", "textures/wall4.tex",
                                      "textures/wall5.tex", "textures/wall6.tex",
                                      "textures/wall7.tex", "textures/wall8.tex"};

    const GameAssetManifest::AssetPackDefinition AssetPacks[] = {
            {
                    GameAssetManager::GAMEASSET_PACKTYPE_INTERNAL,
                    ELEMENTS_OF(InstallFileList),
                    GameAssetManifest::MAIN_ASSETPACK_NAME,
                    InstallFileList
            },
            {
                    GameAssetManager::GAMEASSET_PACKTYPE_ONDEMAND,
                    ELEMENTS_OF(OnDemandFileList),
                    GameAssetManifest::EXPANSION_ASSETPACK_NAME,
                    OnDemandFileList
            }
    };
}

namespace GameAssetManifest {
    size_t AssetManifest_GetAssetPackCount() {
        return ELEMENTS_OF(AssetPacks);
    }

    const AssetPackDefinition *AssetManifest_GetAssetPackDefinitions() {
        return AssetPacks;
    }

}
