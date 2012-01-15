import QtQuick 1.1

Rectangle {
    id: root

    signal clicked()

    property bool active: false
    property alias text: label.text

    radius: 18
    color: "#888888"
    border.color: "black"
    border.width: active ? 3 : 0
    opacity: mouseArea.pressed ? 1.0 : 0.7

    implicitWidth: label.width + 30
    implicitHeight: 40

    Text {
        id: label
        font.pixelSize: 20
        anchors.centerIn: parent
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
