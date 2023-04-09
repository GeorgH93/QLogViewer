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

#include <yaml-cpp/yaml.h>
#include <QString>
#include <QColor>
#include <QPixmap>
#include <QImage>
#include <QBuffer>
#include <QIcon>

#include "LogLevel.h"

namespace YAML
{
#pragma region QT_CONVERTERS
    template<>
    struct convert<QString>
    {
        static Node encode(const QString& str)
        {
            return Node(str.toUtf8().constData());
        }

        static bool decode(const Node& node, QString& str)
        {
            if (node.IsScalar())
            {
                str = QString::fromStdString(node.Scalar());
                return true;
            }
            return false;
        }
    };

	template<>
	struct convert<QStringList>
	{
		static Node encode(const QStringList& stringList)
		{
			Node node;
			for(const auto& string : stringList)
			{
				node.push_back(string);
			}
			return node;
		}

		static bool decode(const Node& node, QStringList& stringList)
		{
			if (node.IsSequence())
			{
				stringList.clear();
				for (const Node& n : node)
				{
					stringList.push_back(n.as<QString>());
				}
				return true;
			}
			return false;
		}
	};

    template<>
    struct convert<QColor>
    {
        static Node encode(const QColor& color)
        {
            return convert<QString>::encode(color.name());
        }

        static bool decode(const Node& node, QColor& color)
        {
            if (node.IsScalar())
            {
                color = QColor(node.Scalar().c_str());
                return true;
            }
            return false;
        }
    };
#pragma endregion
}