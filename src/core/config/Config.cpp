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

#include <algorithm>
#include <cinttypes>
#include <cstring>
#include <uv.h>


#include "core/config/Config.h"
#include "3rdparty/rapidjson/document.h"
#include "backend/cpu/Cpu.h"
#include "base/io/log/Log.h"
#include "base/kernel/interfaces/IJsonReader.h"
#include "base/net/dns/Dns.h"
#include "crypto/common/Assembly.h"


#ifdef TGXM_ALGO_RANDOMX
#   include "crypto/rx/RxConfig.h"
#endif


#ifdef TGXM_FEATURE_OPENCL
#   include "backend/opencl/OclConfig.h"
#endif


#ifdef TGXM_FEATURE_CUDA
#   include "backend/cuda/CudaConfig.h"
#endif


namespace tgxm {


constexpr static uint32_t kIdleTime     = 60U;


const char *Config::kPauseOnBattery     = "pause-on-battery";
const char *Config::kPauseOnActive      = "pause-on-active";


#ifdef TGXM_FEATURE_OPENCL
const char *Config::kOcl                = "opencl";
#endif

#ifdef TGXM_FEATURE_CUDA
const char *Config::kCuda               = "cuda";
#endif

#if defined(TGXM_FEATURE_NVML) || defined (TGXM_FEATURE_ADL)
const char *Config::kHealthPrintTime    = "health-print-time";
#endif

#ifdef TGXM_FEATURE_DMI
const char *Config::kDMI                = "dmi";
#endif


class ConfigPrivate
{
public:
    bool pauseOnBattery = false;
    CpuConfig cpu;
    uint32_t idleTime   = 0;

#   ifdef TGXM_ALGO_RANDOMX
    RxConfig rx;
#   endif

#   ifdef TGXM_FEATURE_OPENCL
    OclConfig cl;
#   endif

#   ifdef TGXM_FEATURE_CUDA
    CudaConfig cuda;
#   endif

#   if defined(TGXM_FEATURE_NVML) || defined (TGXM_FEATURE_ADL)
    uint32_t healthPrintTime = 60U;
#   endif

#   ifdef TGXM_FEATURE_DMI
    bool dmi = true;
#   endif

    void setIdleTime(const rapidjson::Value &value)
    {
        if (value.IsBool()) {
            idleTime = value.GetBool() ? kIdleTime : 0U;
        }
        else if (value.IsUint()) {
            idleTime = value.GetUint();
        }
    }
};

} // namespace tgxm


tgxm::Config::Config() :
    d_ptr(new ConfigPrivate())
{
}


tgxm::Config::~Config()
{
    delete d_ptr;
}


bool tgxm::Config::isPauseOnBattery() const
{
    return d_ptr->pauseOnBattery;
}


const tgxm::CpuConfig &tgxm::Config::cpu() const
{
    return d_ptr->cpu;
}


uint32_t tgxm::Config::idleTime() const
{
    return d_ptr->idleTime * 1000U;
}


#ifdef TGXM_FEATURE_OPENCL
const tgxm::OclConfig &tgxm::Config::cl() const
{
    return d_ptr->cl;
}
#endif


#ifdef TGXM_FEATURE_CUDA
const tgxm::CudaConfig &tgxm::Config::cuda() const
{
    return d_ptr->cuda;
}
#endif


#ifdef TGXM_ALGO_RANDOMX
const tgxm::RxConfig &tgxm::Config::rx() const
{
    return d_ptr->rx;
}
#endif


#if defined(TGXM_FEATURE_NVML) || defined (TGXM_FEATURE_ADL)
uint32_t tgxm::Config::healthPrintTime() const
{
    return d_ptr->healthPrintTime;
}
#endif


#ifdef TGXM_FEATURE_DMI
bool tgxm::Config::isDMI() const
{
    return d_ptr->dmi;
}
#endif


bool tgxm::Config::isShouldSave() const
{
    if (!isAutoSave()) {
        return false;
    }

#   ifdef TGXM_FEATURE_OPENCL
    if (cl().isShouldSave()) {
        return true;
    }
#   endif

#   ifdef TGXM_FEATURE_CUDA
    if (cuda().isShouldSave()) {
        return true;
    }
#   endif

    return (m_upgrade || cpu().isShouldSave());
}


bool tgxm::Config::read(const IJsonReader &reader, const char *fileName)
{
    if (!BaseConfig::read(reader, fileName)) {
        return false;
    }

    d_ptr->pauseOnBattery = reader.getBool(kPauseOnBattery, d_ptr->pauseOnBattery);
    d_ptr->setIdleTime(reader.getValue(kPauseOnActive));

    d_ptr->cpu.read(reader.getValue(CpuConfig::kField));

#   ifdef TGXM_ALGO_RANDOMX
    if (!d_ptr->rx.read(reader.getValue(RxConfig::kField))) {
        m_upgrade = true;
    }
#   endif

#   ifdef TGXM_FEATURE_OPENCL
    if (!pools().isBenchmark()) {
        d_ptr->cl.read(reader.getValue(kOcl));
    }
#   endif

#   ifdef TGXM_FEATURE_CUDA
    if (!pools().isBenchmark()) {
        d_ptr->cuda.read(reader.getValue(kCuda));
    }
#   endif

#   if defined(TGXM_FEATURE_NVML) || defined (TGXM_FEATURE_ADL)
    d_ptr->healthPrintTime = reader.getUint(kHealthPrintTime, d_ptr->healthPrintTime);
#   endif

#   ifdef TGXM_FEATURE_DMI
    d_ptr->dmi = reader.getBool(kDMI, d_ptr->dmi);
#   endif

    return true;
}


void tgxm::Config::getJSON(rapidjson::Document &doc) const
{
    using namespace rapidjson;

    doc.SetObject();

    auto &allocator = doc.GetAllocator();

    Value api(kObjectType);
    api.AddMember(StringRef(kApiId),                    m_apiId.toJSON(), allocator);
    api.AddMember(StringRef(kApiWorkerId),              m_apiWorkerId.toJSON(), allocator);

    doc.AddMember(StringRef(kApi),                      api, allocator);
    doc.AddMember(StringRef(kHttp),                     m_http.toJSON(doc), allocator);
    doc.AddMember(StringRef(kAutosave),                 isAutoSave(), allocator);
    doc.AddMember(StringRef(kBackground),               isBackground(), allocator);
    doc.AddMember(StringRef(kColors),                   Log::isColors(), allocator);
    doc.AddMember(StringRef(kTitle),                    title().toJSON(), allocator);

#   ifdef TGXM_ALGO_RANDOMX
    doc.AddMember(StringRef(RxConfig::kField),          rx().toJSON(doc), allocator);
#   endif

    doc.AddMember(StringRef(CpuConfig::kField),         cpu().toJSON(doc), allocator);

#   ifdef TGXM_FEATURE_OPENCL
    doc.AddMember(StringRef(kOcl),                      cl().toJSON(doc), allocator);
#   endif

#   ifdef TGXM_FEATURE_CUDA
    doc.AddMember(StringRef(kCuda),                     cuda().toJSON(doc), allocator);
#   endif

    doc.AddMember(StringRef(kLogFile),                  m_logFile.toJSON(), allocator);

    m_pools.toJSON(doc, doc);

    doc.AddMember(StringRef(kPrintTime),                printTime(), allocator);
#   if defined(TGXM_FEATURE_NVML) || defined (TGXM_FEATURE_ADL)
    doc.AddMember(StringRef(kHealthPrintTime),          healthPrintTime(), allocator);
#   endif

#   ifdef TGXM_FEATURE_DMI
    doc.AddMember(StringRef(kDMI),                      isDMI(), allocator);
#   endif

    doc.AddMember(StringRef(kSyslog),                   isSyslog(), allocator);

#   ifdef TGXM_FEATURE_TLS
    doc.AddMember(StringRef(kTls),                      m_tls.toJSON(doc), allocator);
#   endif

    doc.AddMember(StringRef(DnsConfig::kField),         Dns::config().toJSON(doc), allocator);
    doc.AddMember(StringRef(kUserAgent),                m_userAgent.toJSON(), allocator);
    doc.AddMember(StringRef(kVerbose),                  Log::verbose(), allocator);
    doc.AddMember(StringRef(kWatch),                    m_watch, allocator);
    doc.AddMember(StringRef(kPauseOnBattery),           isPauseOnBattery(), allocator);
    doc.AddMember(StringRef(kPauseOnActive),            (d_ptr->idleTime == 0U || d_ptr->idleTime == kIdleTime) ? Value(isPauseOnActive()) : Value(d_ptr->idleTime), allocator);
}
