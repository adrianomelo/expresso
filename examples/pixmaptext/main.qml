import QtQuick 1.1
import Expresso 1.0

Item {
    width: 300
    height: 300

    property int counter: 0

    Timer {
        repeat: true
        running: true
        interval: 200
        onTriggered: counter++;
    }

    PixmapText {
        anchors.centerIn: parent
        value: counter
        source: Qt.resolvedUrl("images/digits.png")

        charMap: {
            '0': Qt.rect(0, 0, 19, 26),
            '1': Qt.rect(20, 0, 13, 26),
            '2': Qt.rect(36, 0, 19, 26),
            '3': Qt.rect(59, 0, 19, 26),
            '4': Qt.rect(83, 0, 19, 26),
            '5': Qt.rect(108, 0, 19, 26),
            '6': Qt.rect(134, 0, 19, 26),
            '7': Qt.rect(158, 0, 18, 26),
            '8': Qt.rect(185, 0, 19, 26),
            '9': Qt.rect(210, 0, 19, 26),
        }
    }
}
