#ifndef OSFLISTMODEL_H
#define OSFLISTMODEL_H

#include <QAbstractListModel>
#include "fsbmosf.h"

class OSFListModel : public QAbstractListModel
{
	Q_OBJECT
	
public:
	explicit OSFListModel(QObject *parent = nullptr);
	
	enum
	{
		NameRole = Qt::UserRole,
		PathRole,
		DescriptionRole,
		TypeRole,
		IconSourceRole,
		DirRole
	};
	
	
	// Basic functionality:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	
	virtual QHash<int, QByteArray> roleNames() const override;
	void setFSBModel(FSBMOSF *model);
	void reload();
	
private:
	FSBMOSF *_fsbmOSF;
	QHash<int, QString> _iconsources;
};

#endif // OSFLISTMODEL_H
