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
                  controls.onObservations + "x" + controls.onSamples + " [" + controls.osrate + "]"
              } else {
                  controls.inObservations + "x" + controls.inSamples + " [" + controls.israte + "]"
              }
        color: "black"
    }
}
