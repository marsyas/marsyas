import QtQuick 2.0
import QtQuick.Layouts 1.0

Item {
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
