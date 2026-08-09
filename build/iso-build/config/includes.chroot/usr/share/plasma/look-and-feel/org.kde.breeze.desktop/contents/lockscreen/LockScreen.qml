/*
 * Spike minimal lockscreen for kscreenlocker_greet.
 * Stock Plasma shell lockscreen needs plasma-workspace QML we do not ship;
 * without this file the greeter falls into “The screen locker is broken”.
 * Spec: docs/SECURITY.md (Spike owns lock UX; greeter must still satisfy KWin).
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    // Magical properties / signals expected by kscreenlocker_greet
    property bool viewVisible: true
    property bool suspendToRamSupported: false
    property bool suspendToDiskSupported: false
    property string notification: ""
    signal clearPassword()
    signal notificationRepeated()
    signal suspendToDisk()
    signal suspendToRam()

    anchors.fill: parent

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
                placeholderText: qsTr("Password")
                Layout.preferredWidth: 280
                Layout.alignment: Qt.AlignHCenter
                focus: true
                onAccepted: tryUnlock()
            }

            Button {
                text: qsTr("Unlock")
                Layout.alignment: Qt.AlignHCenter
                onClicked: tryUnlock()
            }
        }
    }

    Timer {
        interval: 1000
        running: true
        repeat: true
        triggeredOnStart: true
        onTriggered: {
            const d = new Date();
            const hh = d.getHours().toString().padStart(2, "0");
            const mm = d.getMinutes().toString().padStart(2, "0");
            clockLabel.text = hh + ":" + mm;
        }
    }

    function tryUnlock() {
        statusLabel.text = "";
        root.notification = "";
        authenticator.startAuthenticating();
        authenticator.respond(passwordBox.text);
    }

    Connections {
        target: authenticator
        function onSucceeded() {
            Qt.quit();
        }
        function onFailed(kind) {
            statusLabel.text = qsTr("Incorrect password");
            passwordBox.clear();
            passwordBox.forceActiveFocus();
        }
        function onErrorMessageChanged() {
            if (authenticator.errorMessage)
                statusLabel.text = authenticator.errorMessage;
        }
        function onInfoMessageChanged() {
            if (authenticator.infoMessage)
                statusLabel.text = authenticator.infoMessage;
        }
    }

    Component.onCompleted: {
        authenticator.startAuthenticating();
        passwordBox.forceActiveFocus();
    }

    onClearPassword: passwordBox.clear()
}
