#include "file_cache.h"

#include "tuningfork_utils.h"

#define LOG_TAG "TuningFork"
#include "Log.h"

#include <cinttypes>
#include <sstream>

namespace tf = tuningfork;

namespace {

extern "C" TFErrorCode FileCacheGet(uint64_t key, CProtobufSerialization* value, void* _self) {
    if (_self==nullptr) return TFERROR_BAD_PARAMETER;
    tf::FileCache* self = static_cast<tf::FileCache*>(_self);
    return self->Get(key, value);
}
extern "C" TFErrorCode FileCacheSet(uint64_t key, const CProtobufSerialization* value,
                                    void* _self) {
    if (_self==nullptr) return TFERROR_BAD_PARAMETER;
    tf::FileCache* self = static_cast<tf::FileCache*>(_self);
    return self->Set(key, value);
}
extern "C" TFErrorCode FileCacheRemove(uint64_t key, void* _self) {
    if (_self==nullptr) return TFERROR_BAD_PARAMETER;
    tf::FileCache* self = static_cast<tf::FileCache*>(_self);
    return self->Remove(key);
}

} // anonymous namespace

namespace tuningfork {

using namespace file_utils;

std::string PathToKey(const std::string& path, uint64_t key) {
    std::stringstream str;
    str << path << "/local_cache_" << key;
    return str.str();
}

FileCache::FileCache(const std::string& path) : path_(path) {
    c_cache_ = TFCache {(void*)this, &FileCacheSet, &FileCacheGet, &FileCacheRemove};
}

TFErrorCode FileCache::Get(uint64_t key, CProtobufSerialization* value) {
    std::lock_guard<std::mutex> lock(mutex_);
    ALOGV("FileCache::Get %" PRIu64, key);
    if (CheckAndCreateDir(path_)) {
        auto key_path = PathToKey(path_, key);
        if (FileExists(key_path)) {
            ALOGV("File exists");
            if (LoadBytesFromFile(key_path, value)) {
                ALOGV("Loaded key %" PRId64 " from %s (%" PRIu32 " bytes)", key,
                      key_path.c_str(), value->size);
                return TFERROR_OK;
            }
        }
        else {
            ALOGV("File does not exist");
        }
    }
    return TFERROR_NO_SUCH_KEY;
}

TFErrorCode FileCache::Set(uint64_t key, const CProtobufSerialization* value) {
    std::lock_guard<std::mutex> lock(mutex_);
    ALOGV("FileCache::Set %" PRIu64, key);
    if (CheckAndCreateDir(path_)) {
        auto key_path = PathToKey(path_, key);
        if (SaveBytesToFile(key_path, value)) {
            ALOGV("Saved key %" PRId64 " to %s (%" PRIu32 " bytes)", key, key_path.c_str(), value->size);
            return TFERROR_OK;
        }
    }
    return TFERROR_NO_SUCH_KEY;
}

TFErrorCode FileCache::Remove(uint64_t key) {
    std::lock_guard<std::mutex> lock(mutex_);
    ALOGV("FileCache::Remove %" PRIu64, key);
    if (CheckAndCreateDir(path_)) {
        auto key_path = PathToKey(path_, key);
        if (DeleteFile(key_path)) {
            ALOGV("Deleted key %" PRId64 " (%s)", key, key_path.c_str());
            return TFERROR_OK;
        }
    }
    return TFERROR_NO_SUCH_KEY;
}

TFErrorCode FileCache::Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    ALOGV("FileCache::Clear");
    if (DeleteDir(path_))
        return TFERROR_OK;
    else
        return TFERROR_BAD_FILE_OPERATION;
}

} // namespace tuningfork
