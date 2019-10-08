import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import JASP.Theme 1.0

Item
{
	id:						rect
	focus:					true
	onActiveFocusChanged:	if(activeFocus) favoriteFilesList.forceActiveFocus()

	MenuHeader
	{
		id:					menuHeader
		headertext:			qsTr("Favorite Files")
	}


	FileList
	{
		id:					favoriteFilesList
		cppModel:			fileMenuModel.favoriteFiles.listModel

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
