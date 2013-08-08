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
            model: parent_system.children
            Item {
                Layout.fillWidth: true
                implicitWidth: 1
                implicitHeight: child.implicitHeight;
                MarSystemItem {
                    anchors.fill: parent
                    id: child
                    property var system: modelData
                }
            }
        }
    }
}
