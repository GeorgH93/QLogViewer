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

#include "LogLevel.h"
#include "YamlConverters.h"

namespace YAML
{
    template<>
    struct convert<LogLevel>
    {
        static Node encode(const LogLevel& level)
        {
            Node node;
            node["LevelName"] = level.GetLevelName();
            node["FontColor"] = level.GetFontColor();
            node["BackgroundColor"] = level.GetBackgroundColor();
            node["Alignment"] = level.GetAlignment();
            return node;
        }

        static bool decode(const Node& node, LogLevel& level)
        {
            static const LogLevel defaults;
            if (node.IsMap())
            {
                if (const auto levelNameNode = node["LevelNode"])
                {
                    if (levelNameNode.IsScalar())
                    {
                        level.SetLevelName(levelNameNode.as<QString>());
                        level.SetFontColor(node["FontColor"].as<QColor>(defaults.GetFontColor()));
                        level.SetBackgroundColor(node["BackgroundColor"].as<QColor>(defaults.GetBackgroundColor()));
                        level.SetAlignment(node["Alignment"].as<int>(defaults.GetAlignment()));
                    }
                }

            }
            return false;
        }
    };

    template<>
    struct convert<std::shared_ptr<LogLevel>>
    {
        static Node encode(const std::shared_ptr<LogLevel>& level)
        {
            return convert<LogLevel>::encode(*level);
        }

        static bool decode(const Node& node, std::shared_ptr<LogLevel>& level)
        {
            static const LogLevel defaults;
            if (node.IsMap())
            {
                if (const auto levelNameNode = node["LevelNode"])
                {
                    if (levelNameNode.IsScalar())
                    {
                        level = std::make_shared<LogLevel>(
                            levelNameNode.as<QString>(),
                            node["FontColor"].as<QColor>(defaults.GetFontColor()),
                            node["BackgroundColor"].as<QColor>(defaults.GetBackgroundColor()),
                            node["Alignment"].as<int>(defaults.GetAlignment())
                            );
                    }
                }

            }
            return false;
        }
    };
}