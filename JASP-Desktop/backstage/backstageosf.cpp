//
// Copyright (C) 2018 University of Amsterdam
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

#include "backstageosf.h"
#include "ui_backstageform.h"

#include <QMessageBox>
#include <QInputDialog>

BackstageOSF::BackstageOSF(QWidget *parent): BackstagePage(parent),
	ui(new Ui::BackstageForm)
{
	ui->setupUi(this);
	
	_osfListModel = new OSFListModel(this);
	_model = new FSBMOSF();
	_osfListModel ->setFSBModel(_model);
	
	ui->QmlContent->rootContext()->setContextProperty("osfListModel", _osfListModel);
	ui->QmlContent->rootContext()->setContextProperty("backstageosf", this);
	ui->QmlContent->setSource(QUrl(QStringLiteral("qrc:/backstage/BackstageOSF.qml")));
	
	connect(_model, SIGNAL(authenticationSuccess()), this, SLOT(updateUserDetails()));
	connect(_model, SIGNAL(authenticationClear()), this, SLOT(updateUserDetails()));
	connect(_model, SIGNAL(entriesChanged()), this, SLOT(resetOSFListModel()));

	_fsBrowser = new FSBrowser(this);
	_fsBrowser->setViewType(FSBrowser::ListView);
	_fsBrowser->setFSModel(_model);
	_fsBrowser->hide();
	
}

bool BackstageOSF::loggedin()
{
	return _mLoggedin;
}

void BackstageOSF::setLoggedin(const bool loggedin)
{
	if (_mLoggedin != loggedin)
	{
		_mLoggedin =  loggedin;
		emit loggedinChanged();
	}
}

void BackstageOSF::setOnlineDataManager(OnlineDataManager *odm)
{
	_odm = odm;
	_model->setOnlineDataManager(_odm);

	//_newFolderButton->setEnabled(_model->isAuthenticated());
	setLoggedin(_model->isAuthenticated());
	//_logoutButton->setVisible(_model->isAuthenticated());
	
	connect(_model, SIGNAL(authenticationSuccess()), this, SLOT(authenticatedHandler()));
}

void BackstageOSF::attemptToConnect()
{
	_model->attemptToConnect();
	setLoggedin(_model->isAuthenticated());
	
}

void BackstageOSF::setCurrentFileName(QString currentFileName)
{
	_currentFileName = currentFileName;
	//_fileNameTextBox->setText(_currentFileName);
}

void BackstageOSF::setMode(FileEvent::FileMode mode)
{
	BackstagePage::setMode(mode);
	//bool visible = (mode == FileEvent::FileSave || mode == FileEvent::FileExportResults || mode == FileEvent::FileExportData);
	//_fileNameContainer->setVisible(visible);
	//_newFolderButton->setVisible(visible);
}

//private slots

void BackstageOSF::notifyDataSetSelected(QString path)
{
	//_fileNameTextBox->setText(QFileInfo(path).fileName());
}


void BackstageOSF::notifyDataSetOpened(QString path)
{
	FSBMOSF::OnlineNodeData nodeData = _model->getNodeData(path);
	openSaveFile(nodeData.nodePath, nodeData.name);
}

void BackstageOSF::saveClicked()
{
	FSBMOSF::OnlineNodeData currentNodeData = _model->currentNodeData();

	if (currentNodeData.canCreateFiles == false)
	{
		if (currentNodeData.level == 0)
			QMessageBox::warning(this, "Projects", "Files cannot be added to the projects list.\n\nTo add a new project please use the online OSF services.");
		else if (currentNodeData.level == 1)
			QMessageBox::warning(this, "Data Providers", "Files cannot be added to a projects data providers list.\n\nTo add a new data provider (eg. google drive) please use the online OSF services.");
		else
			QMessageBox::warning(this, currentNodeData.name, "Files cannot be added to '" + currentNodeData.name + "' for an unknown reason.");
		return;
	}

	///QString filename = _fileNameTextBox->text();
	QString filename = "IMPLEMENT  ME!";

	if (checkEntryName(filename, "File", true) == false)
		return;

	QString path;

	if (_model->hasFileEntry(filename.toLower(), path))
		notifyDataSetOpened(path);
	else
		openSaveFile(currentNodeData.nodePath, filename);
}

void BackstageOSF::openSaveFile(const QString &nodePath, const QString &filename)
{
	bool storedata = (_mode == FileEvent::FileSave || _mode == FileEvent::FileExportResults || _mode == FileEvent::FileExportData);

	FileEvent *event = new FileEvent(this, _mode);

	if (event->setPath(nodePath + "#file://" + filename))
	{
		if (storedata)
		{
			//_breadCrumbs->setEnabled(false);
			//_saveButton->setEnabled(false);
			//_fileNameTextBox->setEnabled(false);
			//_newFolderButton->setEnabled(false);
			//_fileNameTextBox->setText(filename);

			_fsBrowser->StartProcessing();

			connect(event, SIGNAL(completed(FileEvent*)), this, SLOT(openSaveCompleted(FileEvent*)));
		}
	}
	else
	{
		QMessageBox::warning(this, "File Types", event->getLastError());
		event->setComplete(false, "Failed to open file from OSF");
		return;
	}

	emit dataSetIORequest(event);
}

void BackstageOSF::userDetailsReceived()
{
	OnlineUserNode *userNode = qobject_cast<OnlineUserNode*>(sender());

	//_logoutButton->setText("Logout " + userNode->getFullname());

	userNode->deleteLater();
}

void BackstageOSF::openSaveCompleted(FileEvent* event)
{

	//_breadCrumbs->setEnabled(true);
	//_saveButton->setEnabled(true);
	//_fileNameTextBox->setEnabled(true);
	//_newFolderButton->setEnabled(true);

	_fsBrowser->StopProcessing();

	if (event->successful())
		_model->refresh();
}


void BackstageOSF::updateUserDetails()
{
	if (_model->isAuthenticated())
	{
		//_breadCrumbs->setEnabled(true);
		//_saveButton->setEnabled(true);
		//_fileNameTextBox->setEnabled(true);
		//_newFolderButton->setEnabled(true);

		OnlineUserNode *userNode = _odm->getOnlineUserData("https://staging2-api.osf.io/v2/users/me/", "fsbmosf");

		userNode->initialise();

		connect(userNode, SIGNAL(finished()), this, SLOT(userDetailsReceived()));
	}
	else
	{
		//_breadCrumbs->setEnabled(false);
		//_saveButton->setEnabled(false);
		//_fileNameTextBox->setEnabled(false);
		//_newFolderButton->setEnabled(false);
	}
}

void BackstageOSF::newFolderCreated()
{
	OnlineDataNode *node = qobject_cast<OnlineDataNode *>(sender());

	if (node->error())
		QMessageBox::warning(this, "", "An error occured and the folder could not be created.");
	else
		_model->refresh();
}

void BackstageOSF::newFolderClicked()
{
	FSBMOSF::OnlineNodeData currentNodeData = _model->currentNodeData();

	if (currentNodeData.canCreateFolders == false)
	{
		if (currentNodeData.level == 0)
			QMessageBox::warning(this, "Projects", "A new folder cannot be added to the projects list.\n\nTo add a new project please use the online OSF services.");
		else if (currentNodeData.level == 1)
			QMessageBox::warning(this, "Data Providers", "A new folder cannot be added to a projects data providers list.\n\nTo add a new data provider (eg. google drive) please use the online OSF services.");
		else
			QMessageBox::warning(this, currentNodeData.name, "A new folder cannot be added to '" + currentNodeData.name + "' for an unknown reason.");
		return;
	}

	bool ok;
	QString name = "New folder";


	do
	{
		name = QInputDialog::getText(this, "New folder", "New folder name", QLineEdit::Normal, name, &ok);
	}
	while(ok && checkEntryName(name, "Folder", false) == false);


	if (ok)
	{
		emit newFolderRequested(name);

		if (_model->hasFolderEntry(name.toLower()) == false)
		{
			OnlineDataNode *node = _odm->createNewFolderAsync(_model->currentNodeData().nodePath + "#folder://" + name, name, "createNewFolder");
			connect(node, SIGNAL(finished()), this, SLOT(newFolderCreated()));
		}
	}
}


void BackstageOSF::authenticatedHandler()
{
	//_newFolderButton->setEnabled(true);
	//_logoutButton->show();
}

void BackstageOSF::resetOSFListModel()
{
	_osfListModel->reload();
}

void BackstageOSF::logoutClicked()
{
	_model->clearAuthentication();
	//_logoutButton->hide();
	setLoggedin(false);
	_model->refresh();
}



bool BackstageOSF::checkEntryName(QString name, QString entryTitle, bool allowFullStop)
{
	if (name.trimmed() == "")
	{
		QMessageBox::warning(this, "", "Entry name cannot be empty.");
		return false;
	}
	else
	{
		QRegularExpression r("[^\\w\\s" + (QString)(allowFullStop ? "\\.-" : "-") + "]");
		if (r.match(name).hasMatch())
		{
			QMessageBox::warning(this, "", entryTitle + " name can only contain the following characters A-Z a-z 0-9 _ " + (allowFullStop ? ". -" : "-"));
			return false;
		}
	}

	return true;
}
