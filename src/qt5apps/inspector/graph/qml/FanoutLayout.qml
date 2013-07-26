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
            ColumnLayout {
                spacing: 0
                Layout.fillHeight: true
                Loader {
                    Layout.fillWidth: true
                    property var system: modelData
                    sourceComponent: children_component;
                }
            }
        }
    }
}
