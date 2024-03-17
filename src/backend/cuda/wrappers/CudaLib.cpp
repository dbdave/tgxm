/* TGXm
 * Copyright (c) 2018-2021 SChernykh   <https://github.com/SChernykh>
 * Copyright (c) 2016-2021 TGXm       <https://github.com/tgxm>, <support@tgxm.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdexcept>
#include <uv.h>


#include "backend/cuda/wrappers/CudaLib.h"
#include "base/io/Env.h"
#include "base/io/log/Log.h"
#include "base/kernel/Process.h"
#include "crypto/rx/RxAlgo.h"


namespace tgxm {


enum Version : uint32_t
{
    ApiVersion,
    DriverVersion,
    RuntimeVersion
};


static uv_lib_t cudaLib;

#if defined(__APPLE__)
static String defaultLoader = "libtgxm-cuda.dylib";
#elif defined(_WIN32)
static String defaultLoader = "tgxm-cuda.dll";
#else
static String defaultLoader = "libtgxm-cuda.so";
#endif


static const char *kAlloc                               = "alloc";
static const char *kCnHash                              = "cnHash";
static const char *kDeviceCount                         = "deviceCount";
static const char *kDeviceInfo                          = "deviceInfo";
static const char *kDeviceInfo_v2                       = "deviceInfo_v2";
static const char *kDeviceInit                          = "deviceInit";
static const char *kDeviceInt                           = "deviceInt";
static const char *kDeviceName                          = "deviceName";
static const char *kDeviceUint                          = "deviceUint";
static const char *kDeviceUlong                         = "deviceUlong";
static const char *kInit                                = "init";
static const char *kKawPowHash                          = "kawPowHash";
static const char *kKawPowPrepare_v2                    = "kawPowPrepare_v2";
static const char *kKawPowStopHash                      = "kawPowStopHash";
static const char *kLastError                           = "lastError";
static const char *kPluginVersion                       = "pluginVersion";
static const char *kRelease                             = "release";
static const char *kRxHash                              = "rxHash";
static const char *kRxPrepare                           = "rxPrepare";
static const char *kSetJob                              = "setJob";
static const char *kSetJob_v2                           = "setJob_v2";
static const char *kVersion                             = "version";


using alloc_t                                           = nvid_ctx * (*)(uint32_t, int32_t, int32_t);
using cnHash_t                                          = bool (*)(nvid_ctx *, uint32_t, uint64_t, uint64_t, uint32_t *, uint32_t *);
using deviceCount_t                                     = uint32_t (*)();
using deviceInfo_t                                      = bool (*)(nvid_ctx *, int32_t, int32_t, uint32_t, int32_t);
using deviceInfo_v2_t                                   = bool (*)(nvid_ctx *, int32_t, int32_t, const char *, int32_t);
using deviceInit_t                                      = bool (*)(nvid_ctx *);
using deviceInt_t                                       = int32_t (*)(nvid_ctx *, CudaLib::DeviceProperty);
using deviceName_t                                      = const char * (*)(nvid_ctx *);
using deviceUint_t                                      = uint32_t (*)(nvid_ctx *, CudaLib::DeviceProperty);
using deviceUlong_t                                     = uint64_t (*)(nvid_ctx *, CudaLib::DeviceProperty);
using init_t                                            = void (*)();
using kawPowHash_t                                      = bool (*)(nvid_ctx *, uint8_t*, uint64_t, uint32_t *, uint32_t *, uint32_t *);
using kawPowPrepare_v2_t                                = bool (*)(nvid_ctx *, const void *, size_t, const void *, size_t, uint32_t, const uint64_t*);
using kawPowStopHash_t                                  = bool (*)(nvid_ctx *);
using lastError_t                                       = const char * (*)(nvid_ctx *);
using pluginVersion_t                                   = const char * (*)();
using release_t                                         = void (*)(nvid_ctx *);
using rxHash_t                                          = bool (*)(nvid_ctx *, uint32_t, uint64_t, uint32_t *, uint32_t *);
using rxPrepare_t                                       = bool (*)(nvid_ctx *, const void *, size_t, bool, uint32_t);
using setJob_t                                          = bool (*)(nvid_ctx *, const void *, size_t, uint32_t);
using setJob_v2_t                                       = bool (*)(nvid_ctx *, const void *, size_t, const char *);
using version_t                                         = uint32_t (*)(Version);


static alloc_t pAlloc                                   = nullptr;
static cnHash_t pCnHash                                 = nullptr;
static deviceCount_t pDeviceCount                       = nullptr;
static deviceInfo_t pDeviceInfo                         = nullptr;
static deviceInfo_v2_t pDeviceInfo_v2                   = nullptr;
static deviceInit_t pDeviceInit                         = nullptr;
static deviceInt_t pDeviceInt                           = nullptr;
static deviceName_t pDeviceName                         = nullptr;
static deviceUint_t pDeviceUint                         = nullptr;
static deviceUlong_t pDeviceUlong                       = nullptr;
static init_t pInit                                     = nullptr;
static kawPowHash_t pKawPowHash                         = nullptr;
static kawPowPrepare_v2_t pKawPowPrepare_v2             = nullptr;
static kawPowStopHash_t pKawPowStopHash                 = nullptr;
static lastError_t pLastError                           = nullptr;
static pluginVersion_t pPluginVersion                   = nullptr;
static release_t pRelease                               = nullptr;
static rxHash_t pRxHash                                 = nullptr;
static rxPrepare_t pRxPrepare                           = nullptr;
static setJob_t pSetJob                                 = nullptr;
static setJob_v2_t pSetJob_v2                           = nullptr;
static version_t pVersion                               = nullptr;


#define DLSYM(x) if (uv_dlsym(&cudaLib, k##x, reinterpret_cast<void**>(&p##x)) == -1) { throw std::runtime_error(std::string("symbol not found: ") + k##x); }


bool CudaLib::m_initialized = false;
bool CudaLib::m_ready       = false;
String CudaLib::m_error;
String CudaLib::m_loader;


} // namespace tgxm


bool tgxm::CudaLib::init(const char *fileName)
{
    if (!m_initialized) {
        m_initialized = true;
        m_loader      = fileName == nullptr ? defaultLoader : Env::expand(fileName);

        if (!open()) {
            return false;
        }

        try {
            load();
        } catch (std::exception &ex) {
            m_error = (std::string(m_loader) + ": " + ex.what()).c_str();

            return false;
        }

        m_ready = true;
    }

    return m_ready;
}


const char *tgxm::CudaLib::lastError() noexcept
{
    return m_error;
}


void tgxm::CudaLib::close()
{
    uv_dlclose(&cudaLib);
}


bool tgxm::CudaLib::cnHash(nvid_ctx *ctx, uint32_t startNonce, uint64_t height, uint64_t target, uint32_t *rescount, uint32_t *resnonce)
{
    return pCnHash(ctx, startNonce, height, target, rescount, resnonce);
}


bool tgxm::CudaLib::deviceInfo(nvid_ctx *ctx, int32_t blocks, int32_t threads, const Algorithm &algorithm, int32_t dataset_host) noexcept
{
    const Algorithm algo = RxAlgo::id(algorithm);

    if (pDeviceInfo) {
        return pDeviceInfo(ctx, blocks, threads, algo, dataset_host);
    }

    return pDeviceInfo_v2(ctx, blocks, threads, algo.isValid() ? algo.name() : nullptr, dataset_host);
}


bool tgxm::CudaLib::deviceInit(nvid_ctx *ctx) noexcept
{
    return pDeviceInit(ctx);
}


bool tgxm::CudaLib::rxHash(nvid_ctx *ctx, uint32_t startNonce, uint64_t target, uint32_t *rescount, uint32_t *resnonce) noexcept
{
    return pRxHash(ctx, startNonce, target, rescount, resnonce);
}


bool tgxm::CudaLib::rxPrepare(nvid_ctx *ctx, const void *dataset, size_t datasetSize, bool dataset_host, uint32_t batchSize) noexcept
{
    return pRxPrepare(ctx, dataset, datasetSize, dataset_host, batchSize);
}


bool tgxm::CudaLib::kawPowHash(nvid_ctx *ctx, uint8_t* job_blob, uint64_t target, uint32_t *rescount, uint32_t *resnonce, uint32_t *skipped_hashes) noexcept
{
    return pKawPowHash(ctx, job_blob, target, rescount, resnonce, skipped_hashes);
}


bool tgxm::CudaLib::kawPowPrepare(nvid_ctx *ctx, const void* cache, size_t cache_size, const void* dag_precalc, size_t dag_size, uint32_t height, const uint64_t* dag_sizes) noexcept
{
    return pKawPowPrepare_v2(ctx, cache, cache_size, dag_precalc, dag_size, height, dag_sizes);
}


bool tgxm::CudaLib::kawPowStopHash(nvid_ctx *ctx) noexcept
{
    return pKawPowStopHash(ctx);
}


bool tgxm::CudaLib::setJob(nvid_ctx *ctx, const void *data, size_t size, const Algorithm &algorithm) noexcept
{
    const Algorithm algo = RxAlgo::id(algorithm);
    if (pSetJob) {
        return pSetJob(ctx, data, size, algo);
    }

    return pSetJob_v2(ctx, data, size, algo.name());
}


const char *tgxm::CudaLib::deviceName(nvid_ctx *ctx) noexcept
{
    return pDeviceName(ctx);
}


const char *tgxm::CudaLib::lastError(nvid_ctx *ctx) noexcept
{
    return pLastError(ctx);
}


const char *tgxm::CudaLib::pluginVersion() noexcept
{
    return pPluginVersion();
}


int32_t tgxm::CudaLib::deviceInt(nvid_ctx *ctx, DeviceProperty property) noexcept
{
    return pDeviceInt(ctx, property);
}


nvid_ctx *tgxm::CudaLib::alloc(uint32_t id, int32_t bfactor, int32_t bsleep) noexcept
{
    return pAlloc(id, bfactor, bsleep);
}


std::string tgxm::CudaLib::version(uint32_t version)
{
    return std::to_string(version / 1000) + "." + std::to_string((version % 1000) / 10);
}


std::vector<tgxm::CudaDevice> tgxm::CudaLib::devices(int32_t bfactor, int32_t bsleep, const std::vector<uint32_t> &hints) noexcept
{
    const uint32_t count = deviceCount();
    if (!count) {
        return {};
    }

    std::vector<CudaDevice> out;
    out.reserve(count);

    if (hints.empty()) {
        for (uint32_t i = 0; i < count; ++i) {
            CudaDevice device(i, bfactor, bsleep);
            if (device.isValid()) {
                out.emplace_back(std::move(device));
            }
        }
    }
    else {
        for (const uint32_t i : hints) {
            if (i >= count) {
                continue;
            }

            CudaDevice device(i, bfactor, bsleep);
            if (device.isValid()) {
                out.emplace_back(std::move(device));
            }
        }
    }

    return out;
}


uint32_t tgxm::CudaLib::deviceCount() noexcept
{
    return pDeviceCount();
}


uint32_t tgxm::CudaLib::deviceUint(nvid_ctx *ctx, DeviceProperty property) noexcept
{
    return pDeviceUint(ctx, property);
}


uint32_t tgxm::CudaLib::driverVersion() noexcept
{
    return pVersion(DriverVersion);
}


uint32_t tgxm::CudaLib::runtimeVersion() noexcept
{
    return pVersion(RuntimeVersion);
}


uint64_t tgxm::CudaLib::deviceUlong(nvid_ctx *ctx, DeviceProperty property) noexcept
{
    return pDeviceUlong(ctx, property);
}


void tgxm::CudaLib::release(nvid_ctx *ctx) noexcept
{
    pRelease(ctx);
}


bool tgxm::CudaLib::open()
{
    m_error = nullptr;

    if (uv_dlopen(m_loader, &cudaLib) == 0) {
        return true;
    }

#   ifdef TGXM_OS_LINUX
    if (m_loader == defaultLoader) {
        m_loader = Process::location(Process::ExeLocation, m_loader);
        if (uv_dlopen(m_loader, &cudaLib) == 0) {
            return true;
        }
    }
#   endif

    m_error = uv_dlerror(&cudaLib);

    return false;
}


void tgxm::CudaLib::load()
{
    DLSYM(Version);

    const uint32_t api = pVersion(ApiVersion);
    if (api < 3U || api > 4U) {
        throw std::runtime_error("API version mismatch");
    }

    DLSYM(Alloc);
    DLSYM(CnHash);
    DLSYM(DeviceCount);
    DLSYM(DeviceInit);
    DLSYM(DeviceInt);
    DLSYM(DeviceName);
    DLSYM(DeviceUint);
    DLSYM(DeviceUlong);
    DLSYM(Init);
    DLSYM(LastError);
    DLSYM(PluginVersion);
    DLSYM(Release);
    DLSYM(RxHash);
    DLSYM(RxPrepare);
    DLSYM(KawPowHash);
    DLSYM(KawPowPrepare_v2);
    DLSYM(KawPowStopHash);

    if (api == 4U) {
        DLSYM(DeviceInfo);
        DLSYM(SetJob);
    }
    else if (api == 3U) {
        DLSYM(DeviceInfo_v2);
        DLSYM(SetJob_v2);
    }

    pInit();
}
