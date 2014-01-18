import QtQuick 2.0
import QtQuick.Layouts 1.0

Rectangle {
    id: root

    implicitWidth: contents.implicitWidth
    implicitHeight: contents.implicitHeight + input_indicator.implicitHeight + output_indicator.implicitHeight + 4

    color: "transparent"
    border {
        color: global_style.node_border
        width: 1
    }

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
        property bool hasError: bugs.indexOf(system.path) != -1
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

    StateGroup {
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
    }

    StateGroup {
        states: State {
            name: "selected"
            when: the_root.selectedSystem === root
            PropertyChanges {
                target: root
                border.width: 2
                border.color: global_style.selection
            }
            PropertyChanges {
                target: label
                color: global_style.selection
            }
        }
    }

    FlowIndicator {
        id: input_indicator
        output: false;
        controls: system.defaultControls;

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            leftMargin: 2
            topMargin: 2
            rightMargin: 2
        }
        z:1

        onClicked: {
            the_root.inputClicked(system.path);
            selectedPort = this;
        }
    }

    FlowIndicator {
        id: output_indicator
        output: true;
        controls: system.defaultControls;

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            leftMargin: 2
            bottomMargin: 2
            rightMargin: 2
        }
        z:1

        onClicked: {
            the_root.outputClicked(system.path);
            selectedPort = this;
        }
    }

    ColumnLayout {
        id: contents
        anchors {
            left: parent.left
            right: parent.right
            top: input_indicator.bottom
        }
        z: 2

        spacing: 0

        Item {
            implicitWidth: label.implicitWidth + 60
            implicitHeight: label.implicitHeight + 10
            anchors { left: parent.left; right: parent.right }

            Rectangle {
                anchors.fill: parent
                anchors{
                    leftMargin: 2
                    rightMargin: 2
                }
                color: global_style.port_background
            }

            Text {
                color: global_style.node_text
                anchors.centerIn: parent
                id: label
                text: system.name + "  [" + system.type + ']'
                Component.onCompleted: font.pointSize = font.pointSize + 2;
            }

            ExpandButton {
                z: 2
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: label.right
                anchors.leftMargin: 10
                visible: system.hasChildren
                expanded: privateData.expanded
                onClicked: setExpanded(!privateData.expanded)
            }
        }

        Item {
            id: children
            visible: privateData.expanded
            implicitWidth:  children_layout.implicitWidth
            anchors { left: parent.left; right: parent.right }

            Rectangle {
                anchors.fill: parent
                color: "transparent"
                border { color: global_style.node_border }
            }

            Loader {
                id: children_layout
                anchors {
                    left: parent.left
                    right: parent.right
                }
                sourceComponent: layoutComponents.get(system.type);
                property var parent_system: system
                property var parent_system_item: root
            }
        }
    }

    MouseArea {
        id: system_area
        anchors.fill: parent
        z: 0
        onDoubleClicked: {
            setExpanded(!privateData.expanded)
        }
        onClicked: {
            the_root.clicked(system.path);
            the_root.selectedSystem = root;
        }
    }
}
