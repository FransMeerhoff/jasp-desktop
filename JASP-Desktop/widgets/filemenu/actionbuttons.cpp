#include "actionbuttons.h"
#include <QObject>
#include <QtQml>

//property variant actionbuttons:			["Open", "Save","Save As", "Export Results", "Export Data","Sync Data", "Close"]

ActionButtons::ActionButtons(QObject *parent) : QAbstractListModel (parent),
	_data({
		{FileOperation::Open,			"Open",				true},
		{FileOperation::Save,			"Save",				false},
		{FileOperation::SaveAs,			"Save As",			false},
		{FileOperation::ExportResults,	"Export Results",	false},
		{FileOperation::ExportData,		"Export Data",		false},
		{FileOperation::SyncData,		"Sync Data",		false},
		{FileOperation::Close,			"Close",			false},
		{FileOperation::Preferences,	"Preferences",		true}
	})
{
	for(size_t i=0; i<_data.size(); i++)
		_opToIndex[_data[i].operation] = i;

	qmlRegisterUncreatableType<ActionButtons>("FileOperation",1,0,"FileOperation","Impossible create a FileOperation Object");
}



QVariant ActionButtons::data(const QModelIndex &index, int role)	const
{
	if(index.row() < 0 || index.row() > rowCount())
		return QVariant();

	switch(role)
	{
	case Qt::DisplayRole:
	case NameRole:			return _data[size_t(index.row())].name;
	case TypeRole:			return _data[size_t(index.row())].operation;
	case EnabledRole:		return _data[size_t(index.row())].enabled;
	default:				return QVariant();
	}
}


bool ActionButtons::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(index.row() < 0 || index.row() > rowCount() || role != EnabledRole)
		return false;

	bool & ref = _data[size_t(index.row())].enabled;

	if(ref != value.toBool())
	{
		ref = value.toBool();
		emit dataChanged(index, index, {EnabledRole});
	}

	return true;
}

QHash<int, QByteArray>	ActionButtons::roleNames() const
{
	static const QHash<int, QByteArray> roles = {
		{ NameRole,		"nameRole"		},
		{ TypeRole,		"typeRole"		},
		{ EnabledRole,	"enabledRole"	} };

	return roles;
}

void ActionButtons::buttonClicked(int fileOperation)
{
	if(fileOperation < Open || fileOperation > Preferences)
		throw std::runtime_error("This should not be happening!");

	FileOperation op = static_cast<FileOperation>(fileOperation);

	emit buttonClickedSignal(op);
}

void ActionButtons::setEnabled(FileOperation operation, bool enabledState)
{
	setData(index(int(_opToIndex[operation]), 0), enabledState, EnabledRole);
}
