import QtQuick 2.0
import QtQuick.Layouts 1.0

Flickable {
    id: the_root
    width: 500
    height: 500
    contentWidth: width;
    contentHeight: top_system.height

    signal clicked(string path)
    signal inputClicked(string path)
    signal outputClicked(string path)

    property var layoutComponents: MarSystemLayoutFactory {}

    MarSystemItem {
        id: top_system
        anchors {
            left: parent.left
            right: parent.right
        }
        expanded: true
    }
}
