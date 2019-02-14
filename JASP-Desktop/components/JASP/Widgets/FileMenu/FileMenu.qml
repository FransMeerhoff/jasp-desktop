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
		for (var i = 0; i < actionRepeaterId.count; i++)
		{
			var nextActElt = (i < (actionRepeaterId.count - 1) ? actionRepeaterId.itemAt(i+1) : actionRepeaterId.itemAt(0));
			actionRepeaterId.itemAt(i).KeyNavigation.down = nextActElt;
			actionRepeaterId.itemAt(i).KeyNavigation.tab = nextActElt;
			actionRepeaterId.itemAt(i).KeyNavigation.right = resourceRepeaterId.itemAt(0);
		}

		for (var j = 0; j < resourceRepeaterId.count; j++)
		{
			var nextResElt = (j < (resourceRepeaterId.count - 1) ? resourceRepeaterId.itemAt(j+1) : resourceRepeaterId.itemAt(0));
			resourceRepeaterId.itemAt(j).KeyNavigation.down = nextResElt;
			resourceRepeaterId.itemAt(j).KeyNavigation.tab = nextResElt;
			resourceRepeaterId.itemAt(j).KeyNavigation.left = actionRepeaterId.itemAt(0);
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
						updateNavigationKeys()
						itemAt(0).focus = true;
					}

					model:					fileMenuModel.actionButtons

					MenuButton
					{
						id:					actionButton

						property bool ignoreFocusChanged: false
						text:				nameRole
						color:				_pressed || activeFocus ? Theme.buttonColorPressed :	_showHovered ? "transparent"						: "transparent"
						selected:			fileMenuModel.fileoperation === typeRole
						width:				parent.width-6
						height:				action_button_height
						anchors.leftMargin: 3
						anchors.left:		parent.left

						onHoveredChanged:
						{
							if (hovered)
							{
								if (!activeFocus)
								{
									ignoreFocusChanged = true;
									focus = true;
								}
								if (!waitForClickButton(nameRole))
								{
									fileMenuModel.actionButtons.buttonClicked(typeRole);
									updateNavigationKeys();
								}
							}
						}

						onFocusChanged:
						{
							if (activeFocus && !ignoreFocusChanged && !waitForClickButton(nameRole))
							{
								fileMenuModel.actionButtons.buttonClicked(typeRole);
								updateNavigationKeys();
							}
							ignoreFocusChanged = false;
						}

						onClicked:
						{
							if (!activeFocus)
							{
								focus = true;
								ignoreFocusChanged = true;
							}
							else
							{
								fileMenuModel.actionButtons.buttonClicked(typeRole);								
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
						//clickonhover:		true
						width:				parent.width-6
						height:				resource_button_height
						color:				_pressed || activeFocus ? Theme.buttonColorPressed :	_showHovered ? "transparent"						: "transparent"
						anchors.leftMargin: 3
						anchors.left:		parent.left
						selected:			qmlRole === fileMenuModel.resourceButtons.currentQML

						onHoveredChanged:
						{
							if (hovered)
							{
								if (!activeFocus)
								{
									ignoreFocusChanged = true;
									focus = true;
								}
								fileMenuModel.resourceButtonsVisible.clicked(typeRole);
							}
						}

						onFocusChanged:
						{
							if (activeFocus && !ignoreFocusChanged)
							{
								fileMenuModel.resourceButtonsVisible.clicked(typeRole);
							}
							ignoreFocusChanged = false;
						}

						onClicked:
						{
							if (!activeFocus)
							{
								ignoreFocusChanged = true;
								focus = true;
							}
							else
							{
								fileMenuModel.resourceButtonsVisible.clicked(typeRole);							
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
