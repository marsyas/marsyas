import QtQuick 2.0
import QtQuick.Layouts 1.0
import Marsyas 1.0

Item {
    implicitHeight: column.implicitHeight + 40
    implicitWidth: column.implicitWidth + 40

    ColumnLayout {
        id: column
        anchors {
            fill: parent
            margins: 20
        }

        spacing: 20

        Repeater {
            id: child_items
            model: parent_system.children
            Item {
                id: child_item
                anchors.horizontalCenter: parent.horizontalCenter
                implicitWidth: child.implicitWidth;
                implicitHeight: child.implicitHeight;
                MarSystemItem {
                    id: child
                    property var system: modelData
                    onParentShouldExpand: parent_system_item.setExpanded(true)
                }
                ConnectionLine {
                    p1: Qt.point(child_item.width / 2, -19)
                    p2: Qt.point(child_item.width / 2, 0)
                    color: global_style.connection
                }
                Rectangle {
                    width: 5; height: 5;
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        verticalCenter: parent.top
                    }
                    color: global_style.connection
                }
                Rectangle {
                    width: 5; height: 5;
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        verticalCenter: parent.bottom
                    }
                    color: global_style.connection
                }
            }
        }
    }
    ConnectionLine {
        anchors.top: column.bottom
        p1: Qt.point(parent.width / 2, 0)
        p2: Qt.point(parent.width / 2, 20)
        color: global_style.connection
    }
}
