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

#include "InfoAreaEnabledPlainTextEdit.h"
#include <memory>

class LineNumberAreaWidget;

class LineNumberPlainTextEdit : public InfoAreaEnabledPlainTextEdit
{
	Q_OBJECT;

public:
	LineNumberPlainTextEdit(QWidget* parent = nullptr);

	virtual ~LineNumberPlainTextEdit() override;
	
private slots:
	void UpdateLineNumberAreaSize();

private:
	std::unique_ptr<LineNumberAreaWidget> lineNumberWidget;
};