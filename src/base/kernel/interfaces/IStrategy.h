/* TGXm
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2018-2020 SChernykh   <https://github.com/SChernykh>
 * Copyright 2016-2020 TGXm       <https://github.com/tgxm>, <support@tgxm.com>
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

#ifndef TGXM_ISTRATEGY_H
#define TGXM_ISTRATEGY_H


#include <cstdint>


namespace tgxm {


class Algorithm;
class IClient;
class JobResult;
class ProxyUrl;


class IStrategy
{
public:
    virtual ~IStrategy() = default;

    virtual bool isActive() const                      = 0;
    virtual IClient *client() const                    = 0;
    virtual int64_t submit(const JobResult &result)    = 0;
    virtual void connect()                             = 0;
    virtual void resume()                              = 0;
    virtual void setAlgo(const Algorithm &algo)        = 0;
    virtual void setProxy(const ProxyUrl &proxy)       = 0;
    virtual void stop()                                = 0;
    virtual void tick(uint64_t now)                    = 0;
};


} /* namespace tgxm */


#endif // TGXM_ISTRATEGY_H
