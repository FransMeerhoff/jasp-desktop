import QtQuick 2.11
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import JASP.Theme 1.0
import JASP.Widgets 1.0

FocusScope
{
	id:			fileMenu

	function updateNavigationKeys()
	{
		console.log("+++++++ NAVIGATING +++++++ " + actionRepeaterId.count + " Actions : " + resourceRepeaterId.count + " Resources");
		for (var i = 0; i < actionRepeaterId.count; i++)
		{
			var nextActElt = (i < (actionRepeaterId.count - 1) ? actionRepeaterId.itemAt(i+1) : actionRepeaterId.itemAt(0));
			actionRepeaterId.itemAt(i).KeyNavigation.down = nextActElt;
			actionRepeaterId.itemAt(i).KeyNavigation.tab = nextActElt;
			actionRepeaterId.itemAt(i).KeyNavigation.right = resourceRepeaterId.itemAt(0);
			if (nextActElt == null) console.log("ALERT !!!!! Null element");
		}

		for (var j = 0; j < resourceRepeaterId.count; j++)
		{
			var nextResElt = (j < (resourceRepeaterId.count - 1) ? resourceRepeaterId.itemAt(j+1) : resourceRepeaterId.itemAt(0));
			resourceRepeaterId.itemAt(j).KeyNavigation.down = nextResElt;
			resourceRepeaterId.itemAt(j).KeyNavigation.tab = nextResElt;
			resourceRepeaterId.itemAt(j).KeyNavigation.left = actionRepeaterId.itemAt(lastfocus_action);
			if (nextResElt == null) console.log("ALERT !!!!! Null element");
		}

	}

	function waitForClickButton (namerole)
	{
		if (namerole === "Close") return true;
		if (namerole === "Save") return true;
		return false;
	}

	width:		slidePart.width
	height:		600
	z:			1
	visible:	actionMenu.x + actionMenu.width > 0

	property variant resourcesbuttons:		["Recent Files", "Current File", "Computer", "OSF", "Data Library"]
	property int action_button_height:		35 * preferencesModel.uiScale
	property int resource_button_height:	1.5 * action_button_height
	property int colWidths:					150 * preferencesModel.uiScale
	property int lastfocus_action:0
	property int lastfocus_resource:0

	focus: fileMenuModel.visible

	Item
	{
		id:		slidePart
		x:		fileMenuModel.visible ? 0 : -(colWidths * 2)
		width:	(resourceScreen.x + resourceScreen.width)
		height:	fileMenu.height

		Behavior on x { PropertyAnimation { duration: Theme.fileMenuSlideDuration; easing.type: Easing.OutCubic  } }

		MouseArea
		{
			id:				gottaCatchEmAll //Clicks that is
			anchors.fill:	parent
			z:				-6
		}

		Rectangle
		{

			id:				actionMenu
			color:			Theme.uiBackground
			anchors.left:	parent.left
			width:			fileMenu.colWidths //fileMenuModel.visible ?  : 0
			height:			parent.height
			border.width:	1
			border.color:	Theme.uiBorder


			Column
			{
				id:					fileAction
				spacing:			4
				width:				parent.width - Theme.generalAnchorMargin

				anchors
				{
					top:				parent.top
					topMargin:			5
					horizontalCenter:	parent.horizontalCenter
				}

				Repeater
				{

					id: actionRepeaterId

					Component.onCompleted:
					{
						console.log("--------- On Component Completed ---------------")
						updateNavigationKeys()
						itemAt(0).focus = true;
					}

					model:					fileMenuModel.actionButtons

					MenuButton
					{
						id:					actionButton

						property bool ignoreFocusChanged: false
						text:				nameRole
						color:				activeFocus || index===lastfocus_action ? Theme.buttonColorPressed : "transparent"
						selected:			fileMenuModel.fileoperation === typeRole
						width:				parent.width-6
						height:				action_button_height
						anchors.leftMargin: 3
						anchors.left:		parent.left

						onHoveredChanged:
						{
							console.log("--> ONHOVEREDCHANGED ACTION (" + nameRole + ") ActiveFocus :" + activeFocus + " hovered :" + hovered + " ignoreFocusChanged :" +ignoreFocusChanged)
							if (hovered)
							{
								if (!activeFocus)
								{
									ignoreFocusChanged = true;
									focus = true;
									lastfocus_action = index
									console.log("I CHANGE FOCUS ONHOVEREDCHANGED ACTION : ("+ nameRole + ")");
								}
								if (!waitForClickButton(nameRole))
								{
									fileMenuModel.actionButtons.buttonClicked(typeRole);
									console.log("ACTION BUTTON P AFTER ONHOVEREDCHANGED: ("+ nameRole +")");																		
								}
								updateNavigationKeys();
								ignoreFocusChanged = false;
							}
						}

						onFocusChanged:
						{

							console.log("--> ONFOCUSCHANGED ACTION (" + nameRole + ") ActiveFocus :" + activeFocus + " hovered :" + hovered + " ignoreFocusChanged :" +ignoreFocusChanged)


							if (activeFocus && !ignoreFocusChanged )
							{
								lastfocus_action = index
								if (!waitForClickButton(nameRole))
								{
									console.log("ACTION BUTTON P AFTER ONFOCUSCHANGED: ("+ nameRole + ")");
									fileMenuModel.actionButtons.buttonClicked(typeRole);
								}
								updateNavigationKeys();
							}
							console.log("--> LASTOCUSINDEX : " + lastfocus_action)

						}

						onClicked:
						{
							console.log("--> ONCLICKED ACTION (" + nameRole + ") ActiveFocus :" + activeFocus +  " hovered :" + hovered + " ignoreFocusChanged :" +ignoreFocusChanged)
							if (!activeFocus)
							{
								ignoreFocusChanged = true;
								focus = true;							
								console.log("!!!!!!!!!!!!!!!! I CHANGE FOCUS ONCLICKED ACTION  : (" + nameRole + ")");
							}
							else
							{
								fileMenuModel.actionButtons.buttonClicked(typeRole);								
								console.log("ACTION BUTTON P AFTER ONCLICKED : (" + nameRole + ")");
								updateNavigationKeys();

							}
						}
						enabled:	enabledRole
					}
				}
			}
		}

		Rectangle
		{
			id:				resouceMenu
			color:			Theme.uiBackground

			width:			fileMenu.colWidths

			anchors.left:	actionMenu.right
			height:			parent.height

			border.width:	1
			border.color:	Theme.uiBorder

			//Behavior on x { PropertyAnimation { duration: Theme.fileMenuSlideDuration; easing.type: Easing.InOutSine  } }

			Column
			{
				id: resourceLocation

				anchors.top:				parent.top
				anchors.topMargin:			5
				anchors.horizontalCenter:	parent.horizontalCenter
				spacing:					6
				width:						parent.width - Theme.generalAnchorMargin

				Repeater
				{

					id: resourceRepeaterId

					model:					fileMenuModel.resourceButtonsVisible

					MenuButton
					{
						id:					resourceButton

						property bool ignoreFocusChanged: false
						text:				nameRole
						width:				parent.width-6
						height:				resource_button_height
						color:				activeFocus || index==lastfocus_resource ? Theme.buttonColorPressed : "transparent"
						anchors.leftMargin: 3
						anchors.left:		parent.left
						selected:			qmlRole === fileMenuModel.resourceButtons.currentQML

						onHoveredChanged:
						{
							console.log("--> ONHOVEREDCHANGED RESOURCE (" + nameRole + ") ActiveFocus :" + activeFocus + " hovered :" + hovered + " ignoreFocusChanged :" +ignoreFocusChanged)

							if (hovered)
							{
								if (!activeFocus)
								{
									ignoreFocusChanged = true;
									focus = true;
									lastfocus_resource = index
									console.log("I CHANGE FOCUS ONHOVEREDCHANGED RESOURCE : ("+ nameRole +")");
								}

								fileMenuModel.resourceButtonsVisible.clicked(typeRole);
								console.log("RESOURCE BUTTON P AFTER ONHOVEREDCHANGED: ("+ nameRole +")");
								//updateNavigationKeys();

								ignoreFocusChanged = false;
							}
						}

						onFocusChanged:
						{
							console.log("--> ONFOCUSCHANGED RESOURCE (" + nameRole + ") ActiveFocus :" + activeFocus + " hovered :" + hovered + " ignoreFocusChanged :" +ignoreFocusChanged)
							if (activeFocus && !ignoreFocusChanged)
							{
								console.log("RESOURCE BUTTON P AFTER ONFOCUSCHANGED: ("+ nameRole + ")");
								fileMenuModel.resourceButtonsVisible.clicked(typeRole);
								lastfocus_resource = index
							}


						}

						onClicked:
						{
							console.log("--> ONCLICKED RESOURCE (" + nameRole + ") ActiveFocus :" + activeFocus + " hovered :" + hovered + " ignoreFocusChanged :" +ignoreFocusChanged)
							if (!activeFocus)
							{
								ignoreFocusChanged = true;
								focus = true;
								console.log("!!!!!!!!!!!!!! I CHANGE FOCUS ONCLICKED RESOURCE  :" + nameRole + " !!!!!!!!!!!!!!!!!!!!!");

							}
							else
							{
								fileMenuModel.resourceButtonsVisible.clicked(typeRole);							
								console.log("RESOURCE BUTTON P AFTER ONCLICKED : (" + nameRole +")");
								//updateNavigationKeys();

							}

						}

						enabled:			true
					}
				}
			}
		}

		Item
		{
			id:			dropShadow
			y:			0
			x:			resourceScreen.x + resourceScreen.width
			height:		resourceScreen.height
			width:		Theme.shadowRadius
			visible:	resourceScreen.visible

			Rectangle
			{
				anchors.centerIn: parent
				rotation:	-90
				gradient:	Gradient {
					GradientStop { position: 0.0; color: Theme.shadow }
					GradientStop { position: 1.0; color: "transparent" } }
				height:		dropShadow.width
				width:		dropShadow.height
			}
		}

		Rectangle
		{
			id: resourceScreen

			property real	otherColumnsWidth: fileMenu.colWidths * 2

			x:				otherColumnsWidth - (aButtonVisible && fileMenuModel.visible ? 0 : width)
			//width:			Math.min(mainWindowRoot.width - otherColumnsWidth, 800 * preferencesModel.uiScale)
			width:			fileMenu.parent.width - otherColumnsWidth
			height:			parent.height
			visible:		fileMenuModel.visible || x + width > otherColumnsWidth + 1
			anchors.left:	resourceLocation.right

			border.width:	1
			border.color:	Theme.grayDarker
			color:			Theme.uiBackground
			z:				-2

			property bool aButtonVisible:	fileMenuModel.resourceButtons.currentQML !== ''

			Behavior on x { PropertyAnimation { duration: Theme.fileMenuSlideDuration; easing.type: Easing.OutCubic  } }

			onXChanged: if(x + width <= otherColumnsWidth && !fileMenuModel.visible) fileMenuModel.resourceButtons.currentQML = ""

			Loader
			{
				id:					showSelectedSubScreen
				anchors.fill:		parent
				source:				fileMenuModel.resourceButtons.currentQML
			}
		}
	}
}
