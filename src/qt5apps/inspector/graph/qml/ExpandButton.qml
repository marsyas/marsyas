import QtQuick 2.0

MouseArea {
    id: area
    property bool expanded: false
    implicitWidth: 11
    implicitHeight: 11
    hoverEnabled: true

    Rectangle {
        id: background
        anchors.fill: parent
        anchors.margins: -2
        color: global_style.expand_icon_background
        opacity: area.containsMouse ? 1.0 : 0.0
        /*Behavior on opacity {
            NumberAnimation { duration: 200 }
        }*/
    }

    Rectangle {
        width: 1
        anchors {
            top: parent.top
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }

        color: global_style.expand_icon
        visible: !expanded
    }

    Rectangle {
        height: 1
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }

        color: global_style.expand_icon
    }
}
