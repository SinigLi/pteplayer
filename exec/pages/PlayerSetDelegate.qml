// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Window

ItemDelegate {
    id: root
    width: parent !== null? parent.width : 100
    checkable: true

    onClicked: ListView.view.currentIndex = index

    Component.onCompleted: {
        if(index%2!==0)
        {
            background.opacity=0.5;
            background.color=Material.accent;
        }else
        {
            background.opacity=0.3;
            background.color=Material.accent;
        }
    }

    contentItem: ColumnLayout {
        spacing: 0

        RowLayout {
            Label {
                id: nameLabel
                font.pixelSize: Qt.application.font.pixelSize
                text: model.name
            }
            Item {
                Layout.fillWidth: true
            }
            Switch {
                checked: model.activated
                Layout.alignment: Qt.AlignHCenter
                onClicked: model.activated = checked
            }
        }
        RowLayout {
            Layout.fillWidth: true
            id:stuff_set
            visible: root.checked
            RoundButton{
                id:rbtn_tab_stuff
                Layout.fillWidth: true
                text: "六线谱"
                flat: true
                checked: model.tab_stuff_show
                checkable: true
                // Material.background: checked ? Material.accent : "transparent"

                Component.onCompleted: {
                    background.color=Material.accent;
                    background.opacity=checked ? 0.8 : 0.1;
                }
                onClicked:
                {
                    model.tab_stuff_show = !model.tab_stuff_show;
                    checked = model.tab_stuff_show;
                    background.color=Material.accent;
                    background.opacity=checked ? 0.8 : 0.1;
                }
                contentItem: Text {
                    text: rbtn_tab_stuff.text
                    font: rbtn_tab_stuff.font
                    color: "gold"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
            }
            RoundButton{
                id:rbtn_std_stuff
                Layout.fillWidth: true
                text: "五线谱"
                flat: true
                checked: model.std_stuff_show
                checkable: true
                Component.onCompleted: {
                    background.color=Material.accent;
                    background.opacity=checked ? 0.8 : 0.1;
                }
                onClicked:
                {
                    model.std_stuff_show = !model.std_stuff_show;
                    checked = model.std_stuff_show;
                    background.color=Material.accent;
                    background.opacity=checked ? 0.8 : 0.1;
                }

                // Material.background: checked ? Material.accent : "transparent"
                // Material.background.opacity: checked ? 0.8 : 0.3
                // Material.background:
                // {
                //     color:Material.accent
                //     opacity:checked ? 0.8 : 0.3
                // }

                contentItem: Text {
                    text: rbtn_std_stuff.text
                    font: rbtn_std_stuff.font
                    color: "gold"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
            }
            RoundButton{
                id:rbtn_num_stuff
                Layout.fillWidth: true
                text: "简谱"
                flat: true
                checked: model.number_stuff_show
                checkable: true
                Component.onCompleted: {
                    background.color=Material.accent;
                    background.opacity=checked ? 0.8 : 0.1;
                }
                onClicked:
                {
                    model.number_stuff_show = !model.number_stuff_show;
                    checked = model.number_stuff_show;
                    background.color=Material.accent;
                    background.opacity=checked ? 0.8 : 0.1;
                }

                // Material.background: checked ? Material.accent : "transparent"
                // Material.background.opacity: checked ? 0.8 : 0.3
                // Material.background:
                // {
                //     color:Material.accent
                //     opacity:checked ? 0.8 : 0.3
                // }

                contentItem: Text {
                    text: rbtn_num_stuff.text
                    font: rbtn_num_stuff.font
                    color: "gold"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
            }
        }

    }
}
