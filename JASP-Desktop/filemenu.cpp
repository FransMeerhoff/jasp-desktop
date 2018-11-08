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

#include "filemenu.h"
#include "ui_mainwindow.h"

#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include "utilities/settings.h"

FileMenu::FileMenu(QWidget *parent) : QObject(parent)
{
	
	_mode = FileEvent::FileOpen;
	_currentFileType = Utils::FileType::unknown;
	_currentFileReadOnly = false;
	
	_mainWindow = dynamic_cast<MainWindow *>(parent);
	_qquickfilemenu = _mainWindow->ui->quickWidget_FileMenu;
	_qquickfilemenu->rootContext()->setContextProperty("filemenu",this);
	_qquickfilemenu->rootContext()->setContextProperty("filemenuproperties",&fileMenuProperties);
	
	// Recent Files
	_bsRecentFiles = new BackstageRecentFiles(parent, _qquickfilemenu);
	
	// Current  File
	_bsCurrentFile = new BackstageCurrentFile(parent, _qquickfilemenu);
		
	// Computer
	_bsComputer = new BackstageComputer(parent, _qquickfilemenu);
		
	// Osf
	_bsOSF = new BackstageOSF(parent, _qquickfilemenu);
		
	// DataLibrary
	_bsDataLibrary = new BackstageDataLibrary(parent, _qquickfilemenu);
	
	connect(_bsRecentFiles, SIGNAL(dataSetIORequest(FileEvent *)), this, SLOT(dataSetIORequestHandler(FileEvent *)));
	connect(_bsCurrentFile, SIGNAL(dataSetIORequest(FileEvent *)), this, SLOT(dataSetIORequestHandler(FileEvent *)));
	connect(_bsComputer, SIGNAL(dataSetIORequest(FileEvent *)), this, SLOT(dataSetIORequestHandler(FileEvent *)));
	connect(_bsOSF, SIGNAL(dataSetIORequest(FileEvent *)), this, SLOT(dataSetIORequestHandler(FileEvent *)));
	connect(_bsDataLibrary, SIGNAL(dataSetIORequest(FileEvent *)), this, SLOT(dataSetIORequestHandler(FileEvent *)));	
	connect(&_watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(dataFileModifiedHandler(const QString&)));
	
	setFileoperation(Close);

	_buttonsenabled  = QVector<bool> (NoFileActions);
	setEnableButton(FileOperation::Open, true);
	setEnableButton(FileOperation::Save, false);
	setEnableButton(FileOperation::SaveAs, false);
	setEnableButton(FileOperation::ExportResults, false);
	setEnableButton(FileOperation::ExportData, false);
	setEnableButton(FileOperation::SyncData, false);
	setEnableButton(FileOperation::Close, false);
	
	//fileMenuProperties.insert("recentfiles", QVariant(QString("Recent Files")));
	fileMenuProperties.insert("recentfiles_button_visible", QVariant(bool(true)));
	fileMenuProperties.insert("currentfile_button_visible", QVariant(bool(false)));
	fileMenuProperties.insert("current_button_enable", QVariant(bool(false)));
	fileMenuProperties.insert("computer_button_visible", QVariant(bool(true)));
	fileMenuProperties.insert("computer_button_visible", QVariant(bool(true)));
	fileMenuProperties.insert("datalibrary_button_visible", QVariant(bool(true)));
	
	_qquickfilemenu->setSource(QUrl(QString("qrc:/backstage/FileMenu.qml")));	
		
}

//Properties
bool FileMenu::enable_currentfile_button()
{
	return _enable_currentfile_button;
}

void FileMenu::set_enable_currentfile_button(const bool enable)
{
	if (_enable_currentfile_button == enable)
		return;
	_enable_currentfile_button = enable;
	emit enable_currentfile_button_changed();
}

FileMenu::FileOperation FileMenu::fileoperation()
{
	return _fileoperation;
}

void FileMenu::setFileoperation(FileMenu::FileOperation fo)
{
	_fileoperation = fo;
	emit fileoperationChanged();
}

QVector<bool> FileMenu::buttonsenabled()
{
	return _buttonsenabled;
}

void FileMenu::setButtonsenabled(const QVector<bool> &enable)
{
	_buttonsenabled = enable;
	emit buttonsenabledChanged();
}

void FileMenu::setSaveMode(FileEvent::FileMode mode)
{
	_mode = mode;

	_bsComputer->setMode(_mode);
	
	_bsOSF->setMode(_mode);
	_bsOSF->setCurrentFileName(getDefaultOutFileName());
	
	fileMenuProperties.insert("datalibrary_button_visible", QVariant(bool(false)));

	if (_mode == FileEvent::FileOpen)
	{
		fileMenuProperties.insert("currentfile_button_visible", QVariant(bool(false)));
		fileMenuProperties.insert("recentfiles_button_visible", QVariant(bool(true)));
		fileMenuProperties.insert("datalibrary_button_visible", QVariant(bool(true)));	
	}
	else if (_mode == FileEvent::FileSyncData)
	{
		fileMenuProperties.insert("currentfile_button_visible", QVariant(bool(true)));
		fileMenuProperties.insert("recentfiles_button_visible", QVariant(bool(false)));
		set_enable_currentfile_button(!_bsCurrentFile->getCurrentDataFilePath().isEmpty());
	}
	else
	{
		fileMenuProperties.insert("currentfile_button_visible", QVariant(bool(false)));
		fileMenuProperties.insert("recentfiles_button_visible", QVariant(bool(false)));
	}
}


Utils::FileType FileMenu::getCurrentFileType()
{
	return _currentFileType;
}

QString FileMenu::getCurrentFilePath()
{
		return _currentFilePath;
}


bool FileMenu::isCurrentFileReadOnly()
{
	return _currentFileReadOnly;
}


//Public 
//Redirected
void FileMenu::setOnlineDataManager(OnlineDataManager *odm)
{
	_odm = odm;
	_bsOSF->setOnlineDataManager(odm);
}

FileEvent *FileMenu::open(const QString &path)
{
	FileEvent *event = new FileEvent(this, FileEvent::FileOpen);
	event->setPath(path);
	dataSetIORequestHandler(event);

	return event;
}

FileEvent *FileMenu::save()
{
	FileEvent *event;

	if (_currentFileType != Utils::FileType::jasp || _currentFileReadOnly)
	{
		event = _bsComputer->browseSave();
		if (event->isCompleted())
			return event;
	}
	else
	{
		event = new FileEvent(this, FileEvent::FileSave);
		if (!event->setPath(_currentFilePath))
		{
			QMessageBox::warning(_mainWindow, "File Types", event->getLastError());
			event->setComplete(false, "Failed to open file from OSF");
			return event;
		}
	}

	dataSetIORequestHandler(event);

	return event;
}

void FileMenu::sync()
{
	QString path = _bsCurrentFile->getCurrentDataFilePath();

	if (path.isEmpty())
	{
		QString message = "JASP has no associated data file (csv, sav or ods file) to be synchronized with. Do you want to search for such a data file on your computer?\nNB: You can set this data file also via menu File/Sync Data.";
		QMessageBox msgBox(QMessageBox::Question, QString("No associated data file"), message,
						   QMessageBox::Yes|QMessageBox::Cancel);
		int reply = msgBox.exec();
		if (reply == QMessageBox::Cancel)
			return;

		QString caption = "Find Data File";
		QString filter = "Data File (*.csv *.txt *.sav *.ods)";
		path = QFileDialog::getOpenFileName(_mainWindow, caption, "", filter);
	}

	dataSetOpenCurrentRequestHandler(path);
	
}

FileEvent *FileMenu::close()
{
	FileEvent *event = new FileEvent(this, FileEvent::FileClose);
	dataSetIORequestHandler(event);

	return event;
}

void FileMenu::setCurrentActionIndex(int index)
{
	_selectedActionIndex = index;
	//return _buttonGroup->button(index)->setChecked(true);
}


void FileMenu::setCurrentDataFile(const QString &path)
{
	QString currentPath = _bsCurrentFile->getCurrentDataFilePath();
	if (!currentPath.isEmpty())
		_watcher.removePath(currentPath);

	bool setCurrentPath = true;
	bool enableCurrentTab = false;
	if (!path.isEmpty())
	{
		if (checkSyncFileExists(path))
		{
			enableCurrentTab = true;
			int sync = Settings::value(Settings::DATA_AUTO_SYNCHRONIZATION).toInt();
			if (sync > 0)
				_watcher.addPath(path);
		}
		else
			setCurrentPath = false;
	}

	if (setCurrentPath)
		_bsCurrentFile->setCurrentDataFilePath(path);

	//_tabWidget->tabBar()->setTabEnabled(FileLocation::Current, enableCurrentTab);
	
}

void FileMenu::setDataFileWatcher(bool watch)
{
	QString path = _bsCurrentFile->getCurrentFilePath();
	if (!path.isEmpty())
	{
		if (watch && !_bsCurrentFile->isOnlineFile(path))
			_watcher.addPath(path);
		else
			_watcher.removePath(path);
	}
}


QString FileMenu::getDefaultOutFileName()
{
	QString path = getCurrentFilePath();
	QString DefaultOutFileName="";

	if (path != "")
	{
		QString name =  QFileInfo(path).baseName();
		QString ext = QFileInfo(path).suffix();
		switch (_mode)
		{
		case FileEvent::FileSave:
			ext="jasp";
			break;
		case FileEvent::FileExportResults:
			ext="html";
			break;
		case FileEvent::FileExportData:
		case FileEvent::FileGenerateData:
			ext = "csv";
			break;
		default:
			break;
		}
		DefaultOutFileName = name + "." + ext;
	}

	return DefaultOutFileName;
	
}


void FileMenu::dataSetIOCompleted(FileEvent *event)
{
	

	//From OpenSaveWidget	
	if (event->operation() == FileEvent::FileSave || event->operation() == FileEvent::FileOpen)
	{
		if (event->successful())
		{
			//  don't add examples to the recent list
			if (!event->isReadOnly())
			{
				_bsRecentFiles->pushRecentFilePath(event->path());
				_bsComputer->addRecentFolder(event->path());
			}

			if (event->operation() == FileEvent::FileOpen && !event->isReadOnly())
				setCurrentDataFile(event->dataFilePath());

			// all this stuff is a hack
			QFileInfo info(event->path());
			_bsComputer->setFileName(info.baseName());

			_currentFilePath = event->path();
			_currentFileType = event->type();
			_currentFileReadOnly = event->isReadOnly();
			_bsCurrentFile->setCurrentFileInfo(event->path(), event->type(), event->isReadOnly());
			_bsOSF->setProcessing(false);
		}
	}
	else if (event->operation() == FileEvent::FileSyncData)
	{
		if (event->successful())
			setCurrentDataFile(event->dataFilePath());
		else
			std::cout << "Sync failed: " << event->getLastError().toStdString() << std::endl;
	}
	else if (event->operation() == FileEvent::FileClose)
	{
		_bsComputer->clearFileName();
		_currentFilePath = "";
		_currentFileType = Utils::FileType::unknown;
		_currentFileReadOnly = false;
		_bsCurrentFile->setCurrentFileInfo("", Utils::FileType::unknown, false);
		clearSyncData();
	}
		
	
	//From BacksategWidget	
	if (event->successful())
	{
		if (event->operation() == FileEvent::FileOpen)
		{
			
			setEnableButton(FileOperation::Save, event->type() == Utils::FileType::jasp);
			setEnableButton(FileOperation::SaveAs, true);
			setEnableButton(FileOperation::ExportResults, true);
			setEnableButton(FileOperation::ExportData, true);
			setEnableButton(FileOperation::SyncData, true);
			setEnableButton(FileOperation::Close, true);
		}
		else if (event->operation() == FileEvent::FileSave)
		{
			setEnableButton(FileOperation::Save, true);
		}
		else if (event->operation() == FileEvent::FileClose)
		{
			setEnableButton(FileOperation::Save, false);
			setEnableButton(FileOperation::SaveAs, false);
			setEnableButton(FileOperation::ExportResults, false);
			setEnableButton(FileOperation::ExportData, false);
			setEnableButton(FileOperation::SyncData, false);
			setEnableButton(FileOperation::Close, false);			
		}
	}
}

void FileMenu::dataFileModifiedHandler(QString path)
{

	int autoSync = Settings::value(Settings::DATA_AUTO_SYNCHRONIZATION).toInt();
	if (autoSync > 0)
		dataSetOpenCurrentRequestHandler(path);
	
}

void FileMenu::dataSetIORequestHandler(FileEvent *event)
{
	connect(event, SIGNAL(completed(FileEvent*)), this, SLOT(dataSetIOCompleted(FileEvent*)));
	connect(event, SIGNAL(dataFileChanged(QString)), this, SLOT(dataFileModifiedHandler(QString)));
	emit dataSetIORequest(event);
	
}

void FileMenu::dataSetOpenExampleRequestHandler(QString path)
{
	FileEvent *event = new FileEvent(this);
	event->setPath(path);
	event->setReadOnly();

	dataSetIORequestHandler(event);
}

/// ------- Public Slots ------------
///From backstage
void FileMenu::analysisAdded(Analysis *analysis)
{
	setEnableButton(FileOperation::SaveAs, true);
	setEnableButton(FileOperation::ExportResults, true);
}

void FileMenu::setSyncFile(FileEvent *event)
{
	if (event->successful())
	{
		setCurrentDataFile(event->path());
	}
}

void FileMenu::dataAutoSynchronizationChanged(bool on)
{
	setDataFileWatcher(on);
}

void FileMenu::test()
{
	
	//setEnableButton(Open, !_buttonsenabled[Open]);
	//setEnableButton(ExportData, !_buttonsenabled[ExportData]);

	//fileMenuProperties.insert("recentfiles_button_visible", QVariant(bool(false)));

}

void FileMenu::fileOperationClicked(const int &action)
{
	bool cancel;
	
	switch (action)
	{
	case FileOperation::Open:  // Open
		setSaveMode(FileEvent::FileOpen);
		//_tabPages->setCurrentWidget(_openAndSaveWidget);
		break;

	case FileOperation::Save:  // Save
		if (getCurrentFileType() == Utils::FileType::jasp && ! isCurrentFileReadOnly())
			save();
		else
		{
			setCurrentActionIndex(FileOperation::SaveAs);
			setSaveMode(FileEvent::FileSave);
			//_tabPages->setCurrentWidget(_openAndSaveWidget);
			
		}
		cancel = true;
		break;

	case FileOperation::SaveAs:  // Save As
		setSaveMode(FileEvent::FileSave);
		//_tabPages->setCurrentWidget(_openAndSaveWidget);
		break;

	case FileOperation::ExportResults:  // Export Results
		setSaveMode(FileEvent::FileExportResults);
		//_tabPages->setCurrentWidget(_openAndSaveWidget);
		break;

	case FileOperation::ExportData:  // Export Data
		setSaveMode(FileEvent::FileExportData);
		//_tabPages->setCurrentWidget(_openAndSaveWidget);
		break;

	case FileOperation::SyncData:  // Sync Data
		setSaveMode(FileEvent::FileSyncData);
		//_tabPages->setCurrentWidget(_openAndSaveWidget);
		//_openAndSaveWidget->changeTabIfCurrentFileEmpty();
		break;

	case FileOperation::Close: // Close
		close();
		setCurrentActionIndex(FileOperation::Open);
		setSaveMode(FileEvent::FileOpen);
		//_tabPages->setCurrentWidget(_openAndSaveWidget);
		cancel = true;
		break;
	}
}

void FileMenu::resourceButtonClicked(const int &resource)
{

	//if (_selectedIndex == id)
	//	return;

	bool cancel = false;
	//emit currentChanging(id, cancel);

	if (cancel)
	{
		//_buttonGroup->button(_selectedIndex)->setChecked(true);
	}
	else
	{
		///_selectedIndex = id;
		//emit currentChanged(id);
	}
	
	//Check the OSF tab
	if (resource == FileLocation::OSF)
	{
		_bsOSF->attemptToConnect();
	}
}

void FileMenu::dataSetOpenRequestHandler(QString path)
{
	open(path);
}

//Private Slots
//OpenAndSave
void FileMenu::dataSetOpenCurrentRequestHandler(QString path)
{
	if (path.isEmpty())
		return;

	if (checkSyncFileExists(path))
	{
		FileEvent *event = new FileEvent(this, FileEvent::FileSyncData);
		event->setPath(path);

		dataSetIORequestHandler(event);
	}
}


//Private 
//OpenAndSave
bool FileMenu::checkSyncFileExists(const QString &path)
{
	bool exists = path.startsWith("http") ? true : (QFileInfo::exists(path) && Utils::getFileSize(path.toStdString()) > 0);
    if (!exists)
	{
        int attempts = 1;
        while (!exists && attempts < 20)
        {
            Utils::sleep(100);
            attempts++;
            exists = QFileInfo::exists(path) && Utils::getFileSize(path.toStdString()) > 0;
        }
    }
    if (!exists)
    {
        std::cout << "Sync file does not exist: " << path.toStdString() << std::endl;
        std::cout.flush();
		clearSyncData();
	}

	return exists;
}


void FileMenu::clearSyncData()
{
	setDataFileWatcher(false); // must be done before setting the current to empty.
	_bsCurrentFile->setCurrentDataFilePath(QString());
	//_tabWidget->tabBar()->setTabEnabled(FileLocation::Current, false);
}

bool FileMenu::clearOSFFromRecentList(QString path)
{
	return OnlineDataManager::determineProvider(path) != OnlineDataManager::OSF;
}

void FileMenu::setEnableButton(const int &index, const bool &enable)
{
	_buttonsenabled[index] = enable;
	emit buttonsenabledChanged();
}








