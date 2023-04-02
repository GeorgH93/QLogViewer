/*
 *   Copyright (C) 2023 GeorgH93
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
 *   along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "LogFilter.h"
#include "YamlConverters.h"

namespace YAML
{
    template<>
    struct convert<LogFilter>
    {
        static Node encode(const LogFilter& filter)
        {
            Node node;
            //TODO
            return node;
        }

        static bool decode(const Node& node, LogFilter& filter)
        {
            if (node.IsMap())
            {
                //TODO
            }
            return false;
        }
    };

    template<>
    struct convert<std::shared_ptr<LogFilter>>
    {
        static Node encode(const std::shared_ptr<LogFilter>& filter)
        {
            return convert<LogFilter>::encode(*filter);
        }

        static bool decode(const Node& node, std::shared_ptr<LogFilter>& filter)
        {
            auto ptr = std::make_shared<LogFilter>();
            if (convert<LogFilter>::decode(node, *ptr))
            {
                filter = ptr;
                return true;
            }
            return false;
        }
    };
}