import QtQuick 2.0
import QtQuick.Layouts 1.0
import Marsyas 1.0

Item {
    id: root
    implicitHeight: row.implicitHeight + 60
    implicitWidth: row.implicitWidth + 60

    RowLayout {
        id: row
        anchors {
            fill: parent
            margins: 30
        }

        spacing: 30

        Repeater {
            id: child_items
            model: parent_system.children
            Item {
                id: node
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop
                implicitWidth: child.implicitWidth;
                implicitHeight: child.implicitHeight;
                MarSystemItem {
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    id: child
                    property var system: modelData
                    onParentShouldExpand: parent_system_item.setExpanded(true)
                }
                Rectangle {
                    width: 5; height: 5;
                    anchors {
                        horizontalCenter: child.horizontalCenter
                        verticalCenter: parent.top
                    }
                    color: global_style.connection
                }
                Rectangle {
                    width: 5; height: 5;
                    anchors {
                        horizontalCenter: child.horizontalCenter
                        verticalCenter: parent.bottom
                    }
                    color: global_style.connection
                }
                ConnectionLine {
                    p1: { var pt = mapFromItem(root, root.width / 2, 0); Qt.point(pt.x, pt.y) }
                    p2: { var pt = mapFromItem(node, node.width / 2, 0); Qt.point(pt.x, pt.y) }
                    breakpoint: p1.y + 15
                    color: global_style.connection
                }
                ConnectionLine {
                    p1: { var pt = mapFromItem(node, node.width / 2, node.height); Qt.point(pt.x, pt.y) }
                    p2: { var pt = mapFromItem(root, root.width / 2, root.height); Qt.point(pt.x, pt.y) }
                    breakpoint: p2.y - 15
                    color: global_style.connection
                }
            }
        }
    }
}
