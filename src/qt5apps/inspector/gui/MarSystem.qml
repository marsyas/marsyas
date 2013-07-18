import QtQuick 2.0
import QtQuick.Layouts 1.0
import Marsyas 1.0

Rectangle {
    property bool expanded: false
    property bool showControls: false
    property string absolutePath: system.absolutePath

    signal clicked

    id: root

    implicitWidth: parent.width
    Layout.fillWidth: true

    color: "transparent"
    //color: Qt.rgba(0, 0, 0, 0.3);
    //color: Qt.hsla(system.level * 4 % 7 / 6 + 0, 0.4, 0.6, 1)

    border.width: if (mouseArea.containsMouse) {1} else {0}
    border.color: Qt.rgba(0,0,0.2)


    ColumnLayout {
        id: contents
        anchors {
            left: parent.left
            right: parent.right
        }
        spacing: 0

        MouseArea {
            id: mouseArea
            Layout.fillWidth: true
            /*anchors {
                left: parent.left
                right: parent.right
            }*/
            implicitHeight: header.implicitHeight
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            /*onEntered: {
            //console.log("entered: " + label.text)
            root.border.width = 1;
        }
        onExited: {
            //console.log("exited: " + label.text)
            root.border.width = 0;
        }*/

            onDoubleClicked: {
                if (mouse.button == 1) {
                    if (root.MarSystemView.hasChildren)
                        expanded = !expanded;
                }
            }

            onClicked: {
                root.clicked();
            }

            MarSystemHeader {
                id: header
                path: root.MarSystemView.path
                controls: system.defaultControls
                isGroup: root.MarSystemView.hasChildren
                anchors.left: parent.left
                anchors.right: parent.right
            }
        }

        Item {
            id: childrenBox
            visible: false
            Layout.fillWidth: true;
            implicitHeight: children.implicitHeight + 15 + out_flow_indicator.height

            Rectangle {
                anchors {
                    left: children.left
                    right: children.right
                    bottom: children.bottom
                    top: parent.top
                    //margins: 1
                }
                color: Qt.hsla(system.level * 2 % 15 / 14 + 0, 0.4, 0.6, 1)
            }
/*
            anchors {
                left: parent.left
                right: parent.right
            }
*/
            ColumnLayout {
                id: children
                y: 10
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                spacing: 1
            }
        }
    }

    FlowIndicator {
        id: in_flow_indicator
        output: false
        controls: system.defaultControls
        anchors {
            top: parent.top
            left: parent.left
        }
    }

    FlowIndicator {
        id: out_flow_indicator
        output: true
        controls: system.defaultControls
        anchors {
            bottom: parent.bottom
            right: parent.right
        }
    }

    states: [
        State {
            when: !expanded
            PropertyChanges {
                target: childrenBox
                visible: false
            }
            PropertyChanges {
                target: root
                implicitHeight: contents.height
            }
        },
        State {
            when: expanded
            PropertyChanges {
                target: childrenBox
                visible: true
            }
            PropertyChanges {
                target: root
                implicitHeight: contents.height
                //implicitHeight: contents.height + 5 + out_flow_indicator.height
            }
        }
    ]

    MarSystemView.childrenArea: children
}
