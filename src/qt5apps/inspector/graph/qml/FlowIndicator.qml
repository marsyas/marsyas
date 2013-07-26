import QtQuick 2.0

Rectangle {
    property bool output: false
    property var controls

    implicitWidth: label.implicitWidth
    implicitHeight: label.implicitHeight
    color: Qt.rgba(1, 0.8, 0.8, 0.2)
    Text {
        id: label
        text: if (output) {
                  controls.onSamples + " x " + controls.onObservations + " [" + controls.osrate + "]"
              } else {
                  controls.inSamples + " x " + controls.inObservations + " [" + controls.israte + "]"
              }
        color: "black"
    }
}
