import QtQuick 2.0

Rectangle {
    id: root
    property bool output: false
    property var controls

    signal clicked;

    implicitHeight: label.height
    implicitWidth: label.width + 20

    MouseArea {
        id: mouse_area
        anchors.fill: parent
        hoverEnabled:true
        onClicked: root.clicked()
        onEntered: flowInfoVisible = true
        onExited: flowInfoVisible = false
    }

    color: selectedPort === root ? global_style.selection_background : global_style.port_background

    Text {
        anchors.centerIn: parent

        id: label
        color: selectedPort === root ? global_style.selection : global_style.port_text
        text: if (output) {
                  controls.onSamples + " x " + controls.onObservations + " [" + controls.osrate + "]"
              } else {
                  controls.inSamples + " x " + controls.inObservations + " [" + controls.israte + "]"
              }
        opacity: (flowInfoVisible) ? 1.0 : 0.0
        Behavior on opacity {
            SequentialAnimation {
                NumberAnimation { duration: 500; easing.type: Easing.InOutQuad }
            }
        }
    }
}
