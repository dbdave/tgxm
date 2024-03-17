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

#ifndef TGXM_LINUXMEMORY_H
#define TGXM_LINUXMEMORY_H


#include <cstdint>
#include <cstddef>


namespace tgxm {


class LinuxMemory
{
public:
    static bool reserve(size_t size, uint32_t node, size_t hugePageSize);

    static bool write(const char *path, uint64_t value);
    static int64_t read(const char *path);
};


} /* namespace tgxm */


#endif /* TGXM_LINUXMEMORY_H */
