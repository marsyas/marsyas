import QtQuick 2.0
import QtQuick.Layouts 1.0

ColumnLayout {
    id: root
    spacing: 0
    property color color_code:
        Qt.hsla( (system.level + 1) * 2 % 15 / 14, 0.45, 0.7 )

    Component.onCompleted: {
        the_root.systemViews[system.path] = root;
    }

    signal parentShouldExpand();

    QtObject
    {
        id: privateData
        property var expanded: false
    }

    function setExpanded( expanded )
    {
        if (system.hasChildren)
        {
            privateData.expanded = expanded;
        }
        if (expanded)
            parentShouldExpand();
    }

    states: [
        State {
            when: privateData.expanded
            PropertyChanges {
                target: children
                Layout.preferredHeight: children_layout.implicitHeight
            }
        },
        State {
            when: !privateData.expanded
            PropertyChanges {
                target: children
                Layout.preferredHeight: 16
            }
        }
    ]

    FlowIndicator {
        anchors.horizontalCenter: parent.horizontalCenter
        controls: system.defaultControls;
        output: false;
        color: input_area.containsMouse ? Qt.lighter(color_code, 1.2) : color_code
        MouseArea {
            id: input_area
            anchors.fill: parent
            onClicked: the_root.inputClicked(system.path);
            hoverEnabled: true;
        }
    }
    Rectangle {
        id: frame
        color: Qt.darker(color_code, 1.3)//Qt.rgba( color_code.r, color_code.g, color_code.b, 0.6 )
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
                        setExpanded(!privateData.expanded)
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
                    visible: privateData.expanded
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    sourceComponent: layoutComponents.get(system.type);
                    property var parent_system: system
                    property var parent_system_item: root
                }
                Row {
                    id: dotdotdot
                    visible: !privateData.expanded && system.hasChildren
                    anchors.centerIn: parent
                    spacing: 5
                    Repeater {
                        model: 3
                        Rectangle { width: 4; height: 4; color: Qt.lighter(color_code, 1.5) }
                    }
                }
            }
        }
    }
    FlowIndicator {
        anchors.horizontalCenter: parent.horizontalCenter
        controls: system.defaultControls;
        output: true;
        color: output_area.containsMouse ? Qt.lighter(color_code, 1.2) : color_code
        MouseArea {
            id: output_area
            anchors.fill: parent
            onClicked: the_root.outputClicked(system.path);
            hoverEnabled: true;
        }
    }
}
