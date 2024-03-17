/* TGXm
 * Copyright 2018-2021 SChernykh   <https://github.com/SChernykh>
 * Copyright 2016-2021 TGXm       <https://github.com/tgxm>, <support@tgxm.com>
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

#ifndef TGXM_IBASELISTENER_H
#define TGXM_IBASELISTENER_H


#include "base/tools/Object.h"


namespace tgxm {


class Config;


class IBaseListener
{
public:
    TGXM_DISABLE_COPY_MOVE(IBaseListener)

    IBaseListener()             = default;
    virtual ~IBaseListener()    = default;

    virtual void onConfigChanged(Config *config, Config *previousConfig) = 0;
};


} /* namespace tgxm */


#endif // TGXM_IBASELISTENER_H
