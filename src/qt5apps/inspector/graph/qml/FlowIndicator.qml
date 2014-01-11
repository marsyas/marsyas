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
    }

    color: mouse_area.containsMouse ? "blue" : Qt.rgba(0,0,0.6)

    Text {
        anchors.centerIn: parent

        id: label
        text: if (output) {
                  controls.onSamples + " x " + controls.onObservations + " [" + controls.osrate + "]"
              } else {
                  controls.inSamples + " x " + controls.inObservations + " [" + controls.israte + "]"
              }

        color: mouse_area.containsMouse ? "white" : Qt.rgba(0,0,1)
    }
}
