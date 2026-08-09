/*
 * Spike minimal lockscreen for kscreenlocker_greet.
 * Installed as breeze LNF + org.kde.plasma.desktop shell lockscreen so the
 * greeter does not treat Plasma’s outdated shell QML as primary and fall into
 * “The screen locker is broken”.
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property bool viewVisible: true
    property bool suspendToRamSupported: false
    property bool suspendToDiskSupported: false
    property string notification: ""
    signal clearPassword()
    signal notificationRepeated()
    signal suspendToDisk()
    signal suspendToRam()

    width: 800
    height: 600

    Rectangle {
        anchors.fill: parent
        color: "#1a1a2e"

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 16

            Label {
                text: "Spike"
                color: "#ffffff"
                font.pixelSize: 28
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                id: clockLabel
                color: "#ffffff"
                font.pixelSize: 48
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
                text: Qt.formatTime(new Date(), "hh:mm")
            }

            Label {
                id: statusLabel
                color: "#ffaaaa"
                Layout.alignment: Qt.AlignHCenter
                text: root.notification
            }

            TextField {
                id: passwordBox
                echoMode: TextInput.Password
                placeholderText: "Password"
                Layout.preferredWidth: 280
                Layout.alignment: Qt.AlignHCenter
                focus: true
                onAccepted: root.tryUnlock()
            }

            Button {
                text: "Unlock"
                Layout.alignment: Qt.AlignHCenter
                onClicked: root.tryUnlock()
            }
        }
    }

    Timer {
        interval: 1000
        running: true
        repeat: true
        triggeredOnStart: true
        onTriggered: clockLabel.text = Qt.formatTime(new Date(), "hh:mm")
    }

    function tryUnlock() {
        statusLabel.text = ""
        root.notification = ""
        if (typeof authenticator === "undefined" || !authenticator) {
            statusLabel.text = "Authenticator unavailable"
            return
        }
        authenticator.startAuthenticating()
        authenticator.respond(passwordBox.text)
    }

    Connections {
        target: (typeof authenticator !== "undefined") ? authenticator : null
        function onSucceeded() { Qt.quit() }
        function onFailed(kind) {
            statusLabel.text = "Incorrect password"
            passwordBox.clear()
            passwordBox.forceActiveFocus()
        }
        function onErrorMessageChanged() {
            if (authenticator && authenticator.errorMessage)
                statusLabel.text = authenticator.errorMessage
        }
        function onInfoMessageChanged() {
            if (authenticator && authenticator.infoMessage)
                statusLabel.text = authenticator.infoMessage
        }
    }

    Component.onCompleted: {
        if (typeof authenticator !== "undefined" && authenticator)
            authenticator.startAuthenticating()
        passwordBox.forceActiveFocus()
    }

    onClearPassword: passwordBox.clear()
}
