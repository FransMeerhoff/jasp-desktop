import QtQuick 2.0
import QtQuick.Controls 2.2


Rectangle {
	id : osfLogin
	
	width :implicitWidth
	height : 150
	color: "#ececec"
	
	border.width: 1
	border.color: "darkgray"
	
	Label {
		id : loginToOSF
		
		width: implicitWidth
		height: 30
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.leftMargin: 6
		anchors.topMargin: 12
		
		text : "Login to the OSF"
		font.family: "SansSerif"
		font.pixelSize: 16
		color: "black"
	}
	
	Label {
		id : email
		
		width: 80
		height: 30
		anchors.top: loginToOSF.bottom
		anchors.left: parent.left
		anchors.leftMargin: 6
		anchors.topMargin: 6
		
		text : "Email"
		font.family: "SansSerif"
		font.pixelSize: 14
		color: "black"
		verticalAlignment: Text.AlignVCenter
	}
	
	Rectangle{
		
		id: emailInput
		
		anchors.left: email.right
		anchors.leftMargin: 6		
		anchors.top: email.top			
		anchors.right: parent.right
		anchors.rightMargin: 12
		height: email.height
		
		color: "white"			
		border.width: emailText.activeFocus ? 5 : 1
		border.color: emailText.activeFocus ? "lightblue" : "darkgray"
		
		TextInput {
			id: emailText
			anchors.fill: parent
			anchors.leftMargin: 10
			verticalAlignment: Text.AlignVCenter
			
			font.pixelSize: 14
			color: "red"
		}		
	}
	
	Label {
		id : password
		
		height: 30
		width: 80		
		anchors.top: email.bottom
		anchors.left: parent.left
		anchors.leftMargin: 6
		anchors.topMargin: 6
		verticalAlignment: Text.AlignVCenter
		
		text : "Password"
		font.family: "SansSerif"
		font.pixelSize: 14
		color: "black"
		
	}
	
	Rectangle{
		
		id: passwordInput
		
		anchors.left: password.right
		anchors.leftMargin: 6
		anchors.top: password.top
		anchors.right: parent.right
		anchors.rightMargin: 12
		height: password.height
		
		color: "white"			
		border.width: passwordText.activeFocus ? 5 : 1
		border.color: passwordText.activeFocus ? "lightblue" : "darkgray"
		
		TextInput {
			id: passwordText
			
			anchors.fill: parent
			anchors.leftMargin: 10
			verticalAlignment: Text.AlignVCenter
			
			font.pixelSize: 14
			color: "red"		
		}
		
	}
	
	CheckBox {
		id : rememberme
		width: 20
		height: 20
		indicator.width: 20
		indicator.height: 20
		anchors.left: parent.left
		anchors.leftMargin: 6
		anchors.top: passwordInput.bottom
		anchors.topMargin: 12
		
		font.family: "SansSerif"
		font.pixelSize: 14
		text: "Remember me"		
	}
	
	Button {
		id: loginButton
		
		width: 60
		height: 20
		anchors.top: passwordInput.bottom
		anchors.topMargin: 12
		anchors.right: parent.right
		anchors.rightMargin: 12
		
		text : "Login"
		
	}	
}


