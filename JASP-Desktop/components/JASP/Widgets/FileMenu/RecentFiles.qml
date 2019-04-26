import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import JASP.Theme 1.0

FocusScope
{
	id:			rect

	MenuHeader {
		id: menuHeader
		headertext: qsTr("Recent Files")
	}

	FileList {
		id:			recentFilesList

		cppModel:	fileMenuModel.recentFiles.listModel
		hasBreadCrumbs : false
		focus: true

		anchors
		{
			top:			menuHeader.bottom
			left:			menuHeader.left
			right:			menuHeader.right
			bottom:			parent.bottom
			topMargin:		2 * Theme.generalMenuMargin
			bottomMargin:	Theme.generalAnchorMargin
		}
	}

}
