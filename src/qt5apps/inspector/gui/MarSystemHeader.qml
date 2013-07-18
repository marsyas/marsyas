import QtQuick 2.0
import QtQuick.Layouts 1.0
import Marsyas 1.0

Rectangle {
    id: root
    property string path;
    property bool isGroup: false
    property var controls;

    implicitHeight: header_row.implicitHeight + 4
    implicitWidth: header_row.implicitWidth + 10

    //color: "transparent"
    color: Qt.rgba(0, 0, 0, 0.05);
    //border.width: 1
    //border.color: Qt.rgba(0, 0, 0, 0.3);

    RowLayout {
        id: header_row
        spacing: 4
        anchors.fill: parent
        anchors.margins: 2

        Item { Layout.fillWidth: true }

        Rectangle {
            visible: isGroup
            color: "transparent"
            border.width: 1
            border.color: "black"
            width: 10; height: 10
            /*Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.baseline: parent.baseline
            text: "+"
        }*/
            Layout.alignment: Qt.AlignCenter
        }

        Text {
            id: label
            text: path
            //color: "white"
        }

        Item {
            Layout.fillWidth: true
        }
    }
}
