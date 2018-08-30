import QtQuick 2.0
import QtQuick.Controls 2.2

Rectangle
{
	id:rect
	objectName: "rect"
	color: "#ececec"
	
	property bool loggedin : backstageosf.loggedin
	
	Label
	{
		id:headLabel
		
		width:implicitWidth
		height:30
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.leftMargin: 12
		anchors.topMargin: 12
		verticalAlignment: Text.AlignVCenter
		
		text: "Open Science Framework"
		font.family: "SansSerif"
		font.pixelSize: 18
		color: "black"
	}
	
	
	Button {
		id: logoutButton
		
		visible: loggedin
		
		background: Rectangle {
			anchors.fill: parent
			gradient: Gradient {
				GradientStop { position: 0 ; color:  "#e5e5e5" }
				GradientStop { position: 1 ; color:  "white" }
			}
			border.color: "gray"
			border.width: 1
		}
		
		text: "Logout"
		width: 80
		height: 20
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.rightMargin: 12
		anchors.topMargin: 12
		
		onClicked: {
			backstageosf.logoutClicked();		
		}
	}
	
	ToolSeparator
	{
		id: firstSeparator
		anchors.top: headLabel.bottom
		width: rect.width
		orientation: Qt.Horizontal
	}
	
	OSFList {
		id: osfList
		
		visible: loggedin
		
		anchors.top: firstSeparator.bottom
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.leftMargin: 12  //Position datalibrary items
		anchors.topMargin: 6 
		anchors.bottomMargin: 6
		
	}
	
	OSFLogin {
		id: osfLogin
		
		visible: !loggedin
		height: 200
		
		anchors.top: firstSeparator.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.leftMargin: 12
		anchors.topMargin: 6
		anchors.rightMargin: 6
		
	}
	
	ToolSeparator
	{
		id: secondSeparator
		anchors.bottom: linkOSF.top
		width: rect.width
		orientation: Qt.Horizontal
		anchors.bottomMargin: 5
	}
	
	Text {
		id: linkOSF
		
		height: 30
		width: implicitWidth
		anchors.left: parent.left
		anchors.leftMargin: 12
		anchors.bottom: parent.bottom
		textFormat: Text.StyledText
		
		text:'<font color="blue"><u>About the OSF</u></font>'
		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
			onClicked: Qt.openUrlExternally("http://help.osf.io")
		}
	}
	
	Text {
		id: linkRegister
		
		height: 30
		width: implicitWidth
		anchors.left: linkOSF.right 
		anchors.leftMargin: 12
		anchors.bottom: parent.bottom
		textFormat: Text.StyledText
		
		text:'<font color="blue"><u>Register</u></font>'
		MouseArea {
			anchors.fill: parent
			hoverEnabled: true
			cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
			onClicked: Qt.openUrlExternally("https://osf.io")
		}
	}
}
