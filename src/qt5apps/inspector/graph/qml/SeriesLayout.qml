import QtQuick 2.0
import QtQuick.Layouts 1.0

Item {
    implicitHeight: column.implicitHeight + 60
    implicitWidth: column.implicitWidth + 60

    ColumnLayout {
        id: column
        anchors {
            fill: parent
            margins: 30
        }

        spacing: 20

        Repeater {
            id: child_items
            model: parent_system.children
            Item {
                Layout.alignment: Qt.AlignHCenter
                implicitWidth: child.implicitWidth;
                implicitHeight: child.implicitHeight;
                MarSystemItem {
                    id: child
                    property var system: modelData
                    onParentShouldExpand: parent_system_item.setExpanded(true)
                }
            }
        }
    }
}
