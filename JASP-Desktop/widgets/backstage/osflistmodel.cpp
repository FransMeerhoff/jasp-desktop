#include "osflistmodel.h"
#include "fsentrywidget.h"
#include <QFileInfo>
#include <QDir>

OSFListModel::OSFListModel(QObject *parent)
	: QAbstractListModel(parent)
{
	//_fsbmOSF = new FSBMOSF();
	//_fsbmOSF->refresh();
	_iconsources = FSEntryWidget::sourcesIcons();
	
}

int OSFListModel::rowCount(const QModelIndex &parent) const
{
	// For list models only the root node (an invalid parent) should return the list's size. For all
	// other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
	if (parent.isValid())
		return 0;
	
	return _fsbmOSF->entries().count();
}

QVariant OSFListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
		
	//Set and fill the FileSystemEntryList
	FSBMOSF::FileSystemEntryList fileEntryList = _fsbmOSF->entries();
	
	//Get the FileEntry
	FSEntry item = fileEntryList[index.row()];
	
	switch (role)
	{
	case NameRole:
		return QVariant(item.name);
	case PathRole:
		return QVariant(item.path);
	case DescriptionRole:
		return QVariant(item.description);
	case TypeRole:
		return QVariant(item.entryType);
	case IconSourceRole:
		return QVariant("qrc"+_iconsources[item.entryType]);	
	case DirRole:
		{QFileInfo  fi(item.path);
		return QVariant(fi.path() + QDir::separator());}
	default:
		return QVariant(QStringLiteral("Unknown type"));
	}
	
}

QHash<int, QByteArray> OSFListModel::roleNames() const
{	
	QHash<int, QByteArray> names;
	names[NameRole] = "name";
	names[DescriptionRole] ="description";
	names[PathRole] = "path";
	names[TypeRole] ="type";
	names[IconSourceRole] ="iconsource";
	names[DirRole] ="dirpath";
	return names;	
}

void OSFListModel::setFSBModel(FSBMOSF *model)
{
	beginResetModel();
	
	_fsbmOSF = model;
	_fsbmOSF->refresh();
	
	endResetModel();
}

void OSFListModel::reload()
{
	beginResetModel();
	
	//_fsbmOSF->refresh();
	
	endResetModel();
}

