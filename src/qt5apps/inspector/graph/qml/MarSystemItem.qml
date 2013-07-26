import QtQuick 2.0
import QtQuick.Layouts 1.0

ColumnLayout {
    id: root
    spacing: 0
    property color color_code: Qt.rgba(Math.random(), Math.random(), Math.random())
    property bool expanded: false

    states: [
        State {
            when: expanded
            PropertyChanges {
                target: children
                Layout.preferredHeight: children_layout.implicitHeight
            }
        },
        State {
            when: !expanded
            PropertyChanges {
                target: children
                Layout.preferredHeight: 16
            }
        }
    ]

    FlowIndicator {
        Layout.alignment: Qt.AlignHCenter
        controls: system.defaultControls;
        output: false;
        color: color_code
    }
    Rectangle {
        id: frame
        color: Qt.rgba( color_code.r, color_code.g, color_code.b, 0.4 )
        border {
            color: titleArea.containsMouse ? "red" : color_code
            width: 1
        }
        anchors {
            left: parent.left
            right: parent.right
        }

        implicitWidth: contents.implicitWidth
        implicitHeight: contents.implicitHeight

        ColumnLayout {
            id: contents
            anchors.fill: parent
            spacing: 0

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: label.implicitHeight
                color: color_code
                border {
                    width: 1
                    color: titleArea.containsMouse ? "red" : color_code
                }

                Text {
                    anchors.centerIn: parent
                    id: label
                    text: system.name
                }
                MouseArea {
                    id: titleArea
                    hoverEnabled: true
                    anchors.fill: parent
                    onDoubleClicked: {
                        if (system.hasChildren)
                            expanded = !expanded;
                    }
                    onClicked: {
                        the_root.clicked(system.path);
                    }
                }
            }

            Item {
                id: children
                Layout.fillWidth: true
                Loader {
                    id: children_layout
                    visible: expanded
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    sourceComponent: layoutComponents.get(system.type);
                    property var parent_system: system
                    property var children_component: Qt.createComponent("MarSystemItem.qml")
                }
                Row {
                    id: dotdotdot
                    visible: !expanded && system.hasChildren
                    anchors.centerIn: parent
                    spacing: 5
                    Repeater {
                        model: 3
                        Rectangle { width: 4; height: 4; color: Qt.lighter(color_code, 1.5) }
                    }
                }
            }

            //Component.onCompleted: console.log(system.name)
        }
    }
    FlowIndicator {
        Layout.alignment: Qt.AlignHCenter
        controls: system.defaultControls;
        output: true;
        color: color_code
    }
}
