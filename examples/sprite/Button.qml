import QtQuick 1.1

Image {
    id: button

    signal clicked()
    property alias text: label.text

    MouseArea {
        anchors.fill: parent
        onClicked: button.clicked()
    }

    Text {
        id: label
        anchors.top: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
