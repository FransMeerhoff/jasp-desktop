import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQml.Models 2.2
import JASP.Theme 1.0

JASPControl
{
    id: tableView
    controlType: "TableView"
    backgroundRectangle: rectangle
    implicitWidth: parent.implicitWidth
    implicitHeight: 200
    
    property var model
    property var columnNames: []
    property list<TableViewColumn> columns
    property string title
    property var syncModels
    property var components: []
    
    readonly property int rectangleY: rectangle.y
    
    signal comboBoxActivated(int row, string value);
    
    Text {
        id: text
        anchors.top: parent.top
        anchors.left: parent.left
        text: title
        height: title ? 20 : 0
    }    
    
    Rectangle {
        id: rectangle
        anchors.top: text.bottom
        anchors.left: parent.left
        anchors.margins: 4
        height: tableView.height - text.height
        width: form.width
        color: debug ? Theme.debugBackgroundColor : Theme.controlBackgroundColor
        border.width: 1
        border.color: Theme.borderColor
        
        ListView {
            id: gridView
            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }
            height: parent.height
            width: parent.width
            clip: true
            focus: true
            model: tableView.model
    
            delegate: FocusScope {
                id: itemWrapper
                height: 20
                width: gridView.width
                Rectangle {
                    id: itemRectangle
                    property int row: index
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height
                    width: parent.width
                    color: Theme.controlBackgroundColor
                        
                    Text {
                        id: firstColumn
                        text: model.name
                        anchors.verticalCenter: parent.verticalCenter
                        x: 4
                        elide: Text.ElideRight
                        width: parent.width / (1 + columns.length)
                    }
    
                }
                
                Component.onCompleted: {
                    var previousColumn = firstColumn;
                    console.log("Text is: " + firstColumn.text)
                    var length = columns.length
                    for (var i = 0; i < length; i++) {
                        var newControl = components[i].createObject(itemRectangle);
                        newControl.isBound = false;
                        if (newControl.controlType === "ComboBox") {
                            newControl.model = columns[i].model;
                            //newControl.control.activated.connect(function(activatedIndex) { 
                            //    comboBoxActivated( newControl.parent.row, comboModel[activatedIndex]); 
                            //});
                        }
                        newControl.anchors.left = previousColumn.right
                        newControl.anchors.top = previousColumn.top
                        newControl.width = parent.width / (1 + columns.length)
                        previousColumn = newControl;
                    }
                }
                
            }
    
        }        
    }
    
    Component.onCompleted: {
        var length = columns.length
        for (var i = 0; i < length; i++) {
            var columnType = columns[i].type
            var component = Qt.createComponent(columnType + ".qml");
            components.push(component);
        }
    }
    
}
