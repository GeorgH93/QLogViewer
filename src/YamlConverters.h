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

	template<>
	struct convert<QPixmap>
	{
		static Node encode(const QPixmap& pixmap)
		{
			QByteArray data;
			QBuffer buffer(&data);
			buffer.open(QIODevice::WriteOnly);
			pixmap.save(&buffer, "PNG");

			return Node(data.toBase64().data());
		}

		static bool decode(const Node& node, QPixmap& image)
		{
			if (node.IsScalar())
			{
				QByteArray data = QByteArray::fromBase64(QByteArray(node.Scalar().data()));
				return image.loadFromData(data, "PNG");
			}
			return false;
		}
	};

    template<>
    struct convert<QIcon>
    {
        static Node encode(const QIcon& icon)
        {
			Node node;
            for(const auto& size : icon.availableSizes())
            {
                const QPixmap pixmap = icon.pixmap(size);
				node.push_back(pixmap);
            }
            return node;
        }

        static bool decode(const Node& node, QIcon& icon)
        {
            if (node.IsSequence())
            {
				for(const Node& n : node)
				{
					icon.addPixmap(n.as<QPixmap>());
				}
                return true;
            }
            return false;
        }
    };

    template<>
    struct convert<QImage>
    {
        static Node encode(const QImage& image)
        {
            QPixmap pixmap = QPixmap::fromImage(image);
            return Node(pixmap);
        }

        static bool decode(const Node& node, QImage& image)
        {
            if (node.IsScalar())
            {
                QByteArray data = QByteArray::fromBase64(QByteArray(node.Scalar().data()));
                return image.loadFromData(data, "PNG");
            }
            return false;
        }
    };
#pragma endregion
}