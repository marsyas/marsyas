import QtQuick 2.0
import QtQuick.Layouts 1.0

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
            model: parent_system.children
            Item {
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
            }
        }
    }
}
