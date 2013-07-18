import QtQuick 2.0
import QtQuick.Layouts 1.0
import Marsyas 1.0
//import QtQuick.Window 2.0
import "gui"

Flickable {
    //color: Qt.rgba(0.2,0.2,0.2,1)
    id: root
    width: 500
    height: 500
    contentWidth: width;
    contentHeight: tree.height

    signal clicked(string path)

    MarSystemView {
        property color color: Qt.hsla(0, 1, 0.7, 1)
        id: tree
        //anchors.fill: parent
        width: root.width
        height: childrenRect.height
        system: mySystem
        delegate: MarSystem {
            onClicked: root.clicked(absolutePath)
        }
    }
/*
    Rectangle {
        id: info_panel
        property alias name: control_name.text
        property alias value: control_value.text
        visible: false
        //visible: control_area.containsMouse
        //x: 0; y: 12
        radius: 6
        width: control_name.width + control_value.width + 10
        height: control_name.height + control_value.height + 15
        color: Qt.rgba(0.5,0.5,1,0.8)
        Column {
            x: 5; spacing: 5
            Text { id: control_name; } //text: control.name }
            Text { id: control_value; } //text: control.value }
        }
    }
    */
}
