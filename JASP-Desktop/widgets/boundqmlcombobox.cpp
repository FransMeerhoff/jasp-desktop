//
// Copyright (C) 2013-2018 University of Amsterdam
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//

#include "boundqmlcombobox.h"
#include <QQmlProperty>
#include <QQuickItem>
#include <QAbstractListModel>


BoundQMLComboBox::BoundQMLComboBox(QQuickItem* item, AnalysisQMLForm* form) : BoundQMLItem(item, form)
{
	_boundTo = NULL;
	_currentIndex = -1;
	QQuickItem::connect(item, SIGNAL(activated(int)), this, SLOT(comboBoxChangeValueSlot(int)));
}

void BoundQMLComboBox::bindTo(Option *option)
{
	_boundTo = dynamic_cast<OptionList *>(option);

	if (_boundTo != NULL)
	{
		const std::string& currenValue = _boundTo->value();
		const std::vector<std::string>& options = _boundTo->options();
		_currentIndex = -1;
		if (options.size() > 0)
		{
			for (const std::string val : options)
			{
				_currentIndex++;
				if (val == currenValue)
					break;
			}
			if (_currentIndex >= options.size()) _currentIndex = 0;
			_item->setProperty("currentIndex", _currentIndex);
		}
	}
	else
		qDebug() << "could not bind to OptionList in BoundQMLComboBox.cpp";
}

void BoundQMLComboBox::unbind()
{
	
}

void BoundQMLComboBox::setUp()
{
	BoundQMLItem::setUp();
}

Option *BoundQMLComboBox::createOption()
{
	std::vector<std::string> options;
	QVariant variantModel = _item->property("model");
	QObject *objectModel = qvariant_cast<QObject *>(variantModel);
	QAbstractListModel* abstractListModel = dynamic_cast<QAbstractListModel*>(objectModel);
	if (abstractListModel)
	{
		int count = abstractListModel->rowCount();
		for (int i = 0; i < count; i++)
		{
			QString val = abstractListModel->data(abstractListModel->index(i)).toString();
			options.push_back(val.toStdString());
		}
	}
	else
	{
		QStringList stringListModel = variantModel.toStringList();
		for (int i = 0; i < stringListModel.size(); ++i)
		{
			options.push_back(stringListModel.at(i).toStdString());
		}
	}
		
	
	QVariant currentIndexProp = _item->property("currentIndex");
	if (!currentIndexProp.isNull())
		_currentIndex = currentIndexProp.toInt();
	
	if (options.size() == 0)
		_currentIndex = -1;
	else if (_currentIndex >= (int)(options.size()))
		_currentIndex = 0;
	
	std::string selected = "";
	if (_currentIndex >= 0)
		selected = options[_currentIndex];		
	
	return new OptionList(options, selected);
}

void BoundQMLComboBox::comboBoxChangeValueSlot(int index)
{
	if (_boundTo != NULL && _currentIndex != index)
	{
		_currentIndex = index;
		_boundTo->set(index);
	}
}
