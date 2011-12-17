import QtQuick 1.1
import Expresso 1.0

SpriteScene {
    width: 500
    height: 500

    Text {
        id: title
        anchors {
            top: parent.top
            horizontalCenter: parent.horizontalCenter
            topMargin: 50
        }
        font {
            bold: true
            pixelSize: 40
        }
    }

    Sprite {
        id: sprite
        width: 122
        height: 130
        anchors.centerIn: parent

        spriteStates: [
            SpriteState {
                id: walkingState
                frameCount: 6
                frameWidth: 122
                frameHeight: 130
                frameRate: 0.2
                source: Qt.resolvedUrl("images/walking.png")
                onActivated: title.text = qsTr("Walking state")
            },
            SpriteState {
                id: flyingState
                frameCount: 3
                frameWidth: 122
                frameHeight: 130
                frameRate: 0.2
                source: Qt.resolvedUrl("images/flying.png")
                onActivated: title.text = qsTr("Flying state")
            },
            SpriteState {
                id: jumpToFlyState
                frameCount: 1
                frameWidth: 122
                frameHeight: 130
                frameRate: 0.09
                loopCount: 1
                nextState: flyingState
                source: Qt.resolvedUrl("images/jumping.png")
                onActivated: title.text = qsTr("Jumping state")
            },
            SpriteState {
                id: runToFlyState
                frameCount: 6
                frameWidth: 122
                frameHeight: 130
                frameRate: 0.6
                loopCount: 2
                source: Qt.resolvedUrl("images/walking.png")
                nextState: jumpToFlyState
                onActivated: title.text = qsTr("Running state")
            }
        ]
    }

    Row {
        spacing: 50

        anchors {
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
            bottomMargin: 50
        }

        Button {
            text: qsTr("Walking")
            source: "images/btn_walking.png"
            onClicked: sprite.spriteState = walkingState
        }

        Button {
            text: qsTr("Jump to fly")
            source: "images/btn_jumping.png"
            onClicked: sprite.spriteState = jumpToFlyState
        }

        Button {
            text: qsTr("Run to fly")
            source: "images/btn_runfly.png"
            onClicked: sprite.spriteState = runToFlyState
        }
    }
}
