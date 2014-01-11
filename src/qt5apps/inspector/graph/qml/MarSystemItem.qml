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
        anchors {
            left: parent.left
            right: parent.right
        }

        controls: system.defaultControls;
        output: false;
        onClicked: the_root.inputClicked(system.path);

        MouseArea {
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 3
            width: 11
            height: 11
            Rectangle {
                x: 5
                y: 0
                width: 1
                height: 11
                color: "white"
                visible: !privateData.expanded
            }
            Rectangle {
                x: 0
                y: 5
                width: 11
                height: 1
                color: "white"
            }
            visible: system.hasChildren
            onClicked: setExpanded(!privateData.expanded)
        }
    }
    Rectangle {
        id: frame
        color: "transparent"
        border {
            color: Qt.rgba(0,0,0.6)
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
                implicitWidth: label.implicitWidth + 60
                implicitHeight: label.implicitHeight + 10
                color: titleArea.containsMouse ? Qt.rgba(1, 1, 1, 0.1) : "transparent"

                Text {
                    color: "white"
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
                visible: privateData.expanded
                implicitWidth:  children_layout.implicitWidth
                Layout.fillWidth: true
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
    }
    FlowIndicator {
        anchors {
            left: parent.left
            right: parent.right
        }

        controls: system.defaultControls;
        output: true;
        onClicked: the_root.outputClicked(system.path);
    }
}
