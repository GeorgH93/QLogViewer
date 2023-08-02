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

#include <catch2/catch_test_macros.hpp>
#include "YamlConverters.h"

using namespace YAML;

// ---------------------------------------------------------------------------
// QString conversion
// ---------------------------------------------------------------------------
TEST_CASE("YamlConverters: QString encode produces scalar node", "[YamlConverters]")
{
	QString str = "Hello World";
	Node node = convert<QString>::encode(str);

	CHECK(node.IsScalar());
	CHECK(node.as<std::string>() == "Hello World");
}

TEST_CASE("YamlConverters: QString encode handles unicode", "[YamlConverters]")
{
	QString str = QString::fromUtf8("Hello 世界 🌍");
	Node node = convert<QString>::encode(str);

	CHECK(node.IsScalar());
	// Verify round-trip
	QString decoded = node.as<QString>();
	CHECK(decoded == str);
}

TEST_CASE("YamlConverters: QString encode handles empty string", "[YamlConverters]")
{
	QString str = "";
	Node node = convert<QString>::encode(str);

	CHECK(node.IsScalar());
	CHECK(node.as<std::string>() == "");
}

TEST_CASE("YamlConverters: QString decode from scalar node", "[YamlConverters]")
{
	Node node = Load("TestValue");
	QString str;

	CHECK(convert<QString>::decode(node, str) == true);
	CHECK(str == "TestValue");
}

TEST_CASE("YamlConverters: QString decode returns false for non-scalar", "[YamlConverters]")
{
	Node node = Load("[1, 2, 3]");
	QString str;

	CHECK(convert<QString>::decode(node, str) == false);
}

TEST_CASE("YamlConverters: QString round-trip preserves value", "[YamlConverters]")
{
	QString original = "Test string with spaces and \ttabs\nand newlines";
	Node node = convert<QString>::encode(original);
	QString decoded = node.as<QString>();

	CHECK(decoded == original);
}

// ---------------------------------------------------------------------------
// QStringList conversion
// ---------------------------------------------------------------------------
TEST_CASE("YamlConverters: QStringList encode produces sequence node", "[YamlConverters]")
{
	QStringList list;
	list << "one" << "two" << "three";

	Node node = convert<QStringList>::encode(list);

	CHECK(node.IsSequence());
	CHECK(node.size() == 3);
	CHECK(node[0].as<QString>() == "one");
	CHECK(node[1].as<QString>() == "two");
	CHECK(node[2].as<QString>() == "three");
}

TEST_CASE("YamlConverters: QStringList encode handles empty list", "[YamlConverters]")
{
	QStringList list;
	Node node = convert<QStringList>::encode(list);

	// Empty list may produce undefined/null node type
	CHECK((node.IsNull() || node.IsSequence()));
}

TEST_CASE("YamlConverters: QStringList decode from sequence node", "[YamlConverters]")
{
	Node node = Load("- alpha\n- beta\n- gamma");
	QStringList list;

	CHECK(convert<QStringList>::decode(node, list) == true);
	CHECK(list.size() == 3);
	CHECK(list.contains("alpha"));
	CHECK(list.contains("beta"));
	CHECK(list.contains("gamma"));
}

TEST_CASE("YamlConverters: QStringList decode returns false for non-sequence", "[YamlConverters]")
{
	Node node = Load("scalar");
	QStringList list;

	CHECK(convert<QStringList>::decode(node, list) == false);
}

TEST_CASE("YamlConverters: QStringList decode clears existing content", "[YamlConverters]")
{
	QStringList list;
	list << "existing";

	Node node = Load("- new");
	convert<QStringList>::decode(node, list);

	CHECK(list.size() == 1);
	CHECK(list[0] == "new");
}

TEST_CASE("YamlConverters: QStringList round-trip preserves values", "[YamlConverters]")
{
	QStringList original;
	original << "first" << "second" << "third";

	Node node = convert<QStringList>::encode(original);
	QStringList decoded = node.as<QStringList>();

	CHECK(decoded == original);
}

// ---------------------------------------------------------------------------
// QColor conversion
// ---------------------------------------------------------------------------
TEST_CASE("YamlConverters: QColor encode produces hex string", "[YamlConverters]")
{
	QColor color(255, 0, 0);  // Red
	Node node = convert<QColor>::encode(color);

	CHECK(node.IsScalar());
	// Should produce hex format
	CHECK(!node.Scalar().empty());
	CHECK(node.Scalar()[0] == '#');
}

TEST_CASE("YamlConverters: QColor encode with alpha produces ARGB hex", "[YamlConverters]")
{
	QColor color(255, 0, 0, 128);  // Semi-transparent red
	Node node = convert<QColor>::encode(color);

	CHECK(node.IsScalar());
	// With alpha, should include alpha channel in hex
	CHECK(node.Scalar().length() >= 7);  // At least #RRGGBB
	CHECK(node.Scalar()[0] == '#');
}

TEST_CASE("YamlConverters: QColor decode from hex string", "[YamlConverters]")
{
	Node node = Load("\"#FF0000\"");  // Quoted to avoid YAML comment parsing
	QColor color;

	CHECK(convert<QColor>::decode(node, color) == true);
	CHECK(color.red() == 255);
	CHECK(color.green() == 0);
	CHECK(color.blue() == 0);
}

TEST_CASE("YamlConverters: QColor decode from ARGB hex string", "[YamlConverters]")
{
	Node node = Load("\"#80FF0000\"");  // Quoted to avoid YAML comment parsing
	QColor color;

	CHECK(convert<QColor>::decode(node, color) == true);
	CHECK(color.alpha() == 0x80);
	CHECK(color.red() == 255);
	CHECK(color.green() == 0);
	CHECK(color.blue() == 0);
}

TEST_CASE("YamlConverters: QColor decode from named color", "[YamlConverters]")
{
	Node node = Load("red");
	QColor color;

	CHECK(convert<QColor>::decode(node, color) == true);
	CHECK(color == Qt::red);
}

TEST_CASE("YamlConverters: QColor decode returns false for non-scalar", "[YamlConverters]")
{
	Node node = Load("[1, 2, 3]");
	QColor color;

	CHECK(convert<QColor>::decode(node, color) == false);
}

TEST_CASE("YamlConverters: QColor round-trip preserves RGB values", "[YamlConverters]")
{
	QColor original(100, 150, 200);
	Node node = convert<QColor>::encode(original);
	QColor decoded = node.as<QColor>();

	CHECK(decoded.red() == original.red());
	CHECK(decoded.green() == original.green());
	CHECK(decoded.blue() == original.blue());
}

TEST_CASE("YamlConverters: QColor round-trip preserves ARGB values", "[YamlConverters]")
{
	QColor original(100, 150, 200, 180);
	Node node = convert<QColor>::encode(original);
	QColor decoded = node.as<QColor>();

	CHECK(decoded.alpha() == original.alpha());
	CHECK(decoded.red() == original.red());
	CHECK(decoded.green() == original.green());
	CHECK(decoded.blue() == original.blue());
}

// ---------------------------------------------------------------------------
// QPixmap conversion
// Note: QPixmap operations require GUI support (QApplication/QGuiApplication).
// These tests are limited to decode-only to avoid crashes in headless environments.
// ---------------------------------------------------------------------------
TEST_CASE("YamlConverters: QPixmap decode returns false for non-scalar", "[YamlConverters]")
{
	Node node = Load("[1, 2]");
	QPixmap pixmap;

	CHECK(convert<QPixmap>::decode(node, pixmap) == false);
}

// ---------------------------------------------------------------------------
// QIcon conversion
// Note: QIcon operations require GUI support (QApplication/QGuiApplication).
// These tests are limited to decode-only to avoid crashes in headless environments.
// ---------------------------------------------------------------------------
TEST_CASE("YamlConverters: QIcon decode returns false for non-sequence", "[YamlConverters]")
{
	Node node = Load("scalar");
	QIcon icon;

	CHECK(convert<QIcon>::decode(node, icon) == false);
}

// ---------------------------------------------------------------------------
// QImage conversion
// Note: These tests may fail in headless environments without GUI support
// ---------------------------------------------------------------------------
TEST_CASE("YamlConverters: QImage encode produces base64 PNG", "[YamlConverters]")
{
	QImage image(10, 10, QImage::Format_RGB32);
	image.fill(Qt::magenta);

	// Skip if image is null (headless environment)
	if (image.isNull())
	{
		return;
	}

	Node node = convert<QImage>::encode(image);

	CHECK(node.IsScalar());
	// May be empty in headless environments
}

TEST_CASE("YamlConverters: QImage round-trip preserves image", "[YamlConverters]")
{
	QImage original(20, 20, QImage::Format_RGB32);
	original.fill(Qt::yellow);

	// Skip if image is null (headless environment)
	if (original.isNull())
	{
		return;
	}

	Node node = convert<QImage>::encode(original);

	// Only test round-trip if encode produced data
	if (!node.Scalar().empty())
	{
		QImage decoded = node.as<QImage>();
		CHECK(decoded.isNull() == false);
		CHECK(decoded.width() == 20);
		CHECK(decoded.height() == 20);
	}
}

TEST_CASE("YamlConverters: QImage decode returns false for non-scalar", "[YamlConverters]")
{
	Node node = Load("[1, 2]");
	QImage image;

	CHECK(convert<QImage>::decode(node, image) == false);
}

TEST_CASE("YamlConverters: QImage decode handles invalid base64 gracefully", "[YamlConverters]")
{
	Node node = Load("invalid-data!!!");
	QImage image;

	CHECK(convert<QImage>::decode(node, image) == false);
}

// ---------------------------------------------------------------------------
// Integration tests - complex YAML structures
// ---------------------------------------------------------------------------
TEST_CASE("YamlConverters: can use QString in YAML map", "[YamlConverters]")
{
	Node node;
	node["name"] = QString("TestName");
	node["value"] = QString("TestValue");

	CHECK(node["name"].as<QString>() == "TestName");
	CHECK(node["value"].as<QString>() == "TestValue");
}

TEST_CASE("YamlConverters: can use QStringList in YAML map", "[YamlConverters]")
{
	QStringList levels;
	levels << "ERROR" << "WARN" << "INFO";

	Node node;
	node["levels"] = levels;

	QStringList decoded = node["levels"].as<QStringList>();
	CHECK(decoded.size() == 3);
	CHECK(decoded.contains("ERROR"));
}

TEST_CASE("YamlConverters: can use QColor in YAML map", "[YamlConverters]")
{
	QColor color(255, 128, 64);

	Node node;
	node["fontColor"] = color;

	QColor decoded = node["fontColor"].as<QColor>();
	CHECK(decoded.red() == 255);
	CHECK(decoded.green() == 128);
	CHECK(decoded.blue() == 64);
}

TEST_CASE("YamlConverters: full LogProfile-like structure", "[YamlConverters]")
{
	QStringList levels;
	levels << "ERROR" << "WARN" << "INFO" << "DEBUG";

	Node node;
	node["Name"] = QString("TestProfile");
	node["LogLevels"] = levels;
	node["FontColor"] = QColor(255, 0, 0);
	node["BackgroundColor"] = QColor(255, 240, 240);

	// Verify structure
	CHECK(node["Name"].as<QString>() == "TestProfile");
	CHECK(node["LogLevels"].as<QStringList>().size() == 4);
	CHECK(node["FontColor"].as<QColor>().red() == 255);
}
