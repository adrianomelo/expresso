import QtQuick 1.1
import Expresso 1.0

Item {
    id: root
    width: 600
    height: 300

    property real volume: 1.0
    property bool muted: false
    property bool paused: false


    PcmSound {
        id: hitSound
        muted: root.muted
        volume: root.volume
        paused: root.paused
        source: Qt.resolvedUrl("sounds/hit.wav")
    }

    PcmSound {
        id: windSound
        muted: root.muted
        volume: root.volume
        paused: root.paused
        loops: PcmSound.Infinite
        source: Qt.resolvedUrl("sounds/wind.wav")
    }

    PcmSound {
        id: songSound
        muted: root.muted
        volume: root.volume
        paused: root.paused
        loops: PcmSound.Infinite
        source: Qt.resolvedUrl("sounds/song.wav")
        Component.onCompleted: play()
    }

    Column {
        spacing: 30
        anchors.centerIn: parent

        Row {
            spacing: 20
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: qsTr("song")
                color: "#234134"
                active: songSound.playing
                onClicked: {
                    if (!songSound.playing)
                        songSound.play();
                    else
                        songSound.stop();
                }
            }

            Button {
                text: qsTr("wind")
                color: "#234134"
                active: windSound.playing
                onClicked: {
                    if (!windSound.playing)
                        windSound.play();
                    else
                        windSound.stop();
                }
            }

            Button {
                text: qsTr("hit")
                color: "#234134"
                active: hitSound.playing
                onClicked: {
                    if (hitSound.playing)
                        hitSound.stop();

                    hitSound.play();
                }
            }
        }

        Row {
            spacing: 20
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: qsTr("paused")
                active: root.paused
                onClicked: root.paused = !root.paused
            }

            Button {
                text: qsTr("muted")
                active: root.muted
                onClicked: root.muted = !root.muted
            }

            Button {
                text: qsTr("low")
                active: root.volume == 0.6
                onClicked: root.volume = 0.6
            }
            Button {
                text: qsTr("medium")
                active: root.volume == 0.8
                onClicked: root.volume = 0.8
            }
            Button {
                text: qsTr("high")
                active: root.volume == 1.0
                onClicked: root.volume = 1.0
            }
        }
    }
}
