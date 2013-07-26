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
            ColumnLayout {
                spacing: 0
                Layout.fillWidth: true
                Loader {
                    Layout.fillWidth: true
                    property var system: modelData
                    sourceComponent: children_component;
                }
            }
        }
    }
}
