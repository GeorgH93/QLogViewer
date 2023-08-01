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
            node["Name"] = filter.GetFilterName();
            node["Enabled"] = filter.IsEnabled();
            node["Mode"] = filter.GetFilterMode() == LogFilter::FilterMode::Include ? "Include" : "Exclude";

            if (!filter.GetIncludedLevels().isEmpty())
            {
                node["IncludedLevels"] = filter.GetIncludedLevels();
            }
            if (!filter.GetExcludedLevels().isEmpty())
            {
                node["ExcludedLevels"] = filter.GetExcludedLevels();
            }

            if (!filter.GetSearchText().isEmpty())
            {
                node["SearchText"] = filter.GetSearchText();
                node["CaseSensitive"] = filter.IsCaseSensitive();

                QString matchMode;
                switch (filter.GetTextMatchMode())
                {
                    case LogFilter::TextMatchMode::Contains:   matchMode = "Contains"; break;
                    case LogFilter::TextMatchMode::StartsWith: matchMode = "StartsWith"; break;
                    case LogFilter::TextMatchMode::EndsWith:   matchMode = "EndsWith"; break;
                    case LogFilter::TextMatchMode::ExactMatch: matchMode = "ExactMatch"; break;
                    case LogFilter::TextMatchMode::Regex:      matchMode = "Regex"; break;
                }
                node["TextMatchMode"] = matchMode;
            }

            return node;
        }

        static bool decode(const Node& node, LogFilter& filter)
        {
            if (!node.IsMap())
            {
                return false;
            }

            if (node["Name"])
            {
                filter.SetFilterName(node["Name"].as<QString>());
            }

            if (node["Enabled"])
            {
                filter.SetEnabled(node["Enabled"].as<bool>());
            }

            if (node["Mode"])
            {
                QString mode = node["Mode"].as<QString>();
                filter.SetFilterMode(mode == "Include" ? LogFilter::FilterMode::Include
                                                         : LogFilter::FilterMode::Exclude);
            }

            if (node["IncludedLevels"])
            {
                filter.SetIncludedLevels(node["IncludedLevels"].as<QStringList>());
            }

            if (node["ExcludedLevels"])
            {
                filter.SetExcludedLevels(node["ExcludedLevels"].as<QStringList>());
            }

            if (node["SearchText"])
            {
                filter.SetSearchText(node["SearchText"].as<QString>());
            }

            if (node["CaseSensitive"])
            {
                filter.SetCaseSensitive(node["CaseSensitive"].as<bool>());
            }

            if (node["TextMatchMode"])
            {
                QString mode = node["TextMatchMode"].as<QString>();
                if (mode == "Contains")   filter.SetTextMatchMode(LogFilter::TextMatchMode::Contains);
                else if (mode == "StartsWith") filter.SetTextMatchMode(LogFilter::TextMatchMode::StartsWith);
                else if (mode == "EndsWith")   filter.SetTextMatchMode(LogFilter::TextMatchMode::EndsWith);
                else if (mode == "ExactMatch") filter.SetTextMatchMode(LogFilter::TextMatchMode::ExactMatch);
                else if (mode == "Regex")      filter.SetTextMatchMode(LogFilter::TextMatchMode::Regex);
            }

            return true;
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