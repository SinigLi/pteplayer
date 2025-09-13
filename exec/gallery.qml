// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

pragma ComponentBehavior: Bound

import Scores

import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "." as App

ApplicationWindow {
    id: window
    width: 360
    height: 520
    visible: true
    title: qsTr("Qt Quick Controls")
    //! [orientation]
    readonly property bool portraitMode: !orientationCheckBox.checked || window.width < window.height
    //! [orientation]

    function help() {
        // let displayingControl = listView.currentIndex !== -1
        // let currentControlName = displayingControl
        //     ? listView.model.get(listView.currentIndex).title.toLowerCase() : ""
        // let url = "https://doc.qt.io/qt-6/"
        //     + (displayingControl
        //        ? "qml-qtquick-controls2-" + currentControlName + ".html"
        //        : "qtquick-controls2-qmlmodule.html");
        // Qt.openUrlExternally(url)
    }

    required property var builtInStyles
    // required property var curScoreGroups
    function reloadScoreLs(groupname)
    {
        var scoreLs = score_man.getScoreNamesByGroup(groupname);
        if(scoreLs.length===0){
            log_label.text=qsTr("乐谱组数据列表为空:")+groupname;
            score_list.model=[];
            return;
        }
        log_label.text=qsTr("请双击选中对应乐谱进入演奏模式：");
        score_list.model=scoreLs;
    }
    function reloadScoreLsFilter(groupname,filter)
    {
        var scoreLs = score_man.getScoreNamesByGroupFilter(groupname,filter);
        if(scoreLs.length===0){
            log_label.text=qsTr("乐谱组数据列表为空:")+groupname;
            score_list.model=[];
            return;
        }
        log_label.text=qsTr("请双击选中对应乐谱进入演奏模式：");
        score_list.model=scoreLs;
    }
    function resetScoreList() {

        score_groups.model=score_man.scoreGroups();
        // if(score_groups.count===0)
        // {
        // }

        if(score_groups.count===0)
        {
            log_label.text=qsTr("未发现有效乐谱文件，请手动选择乐谱文件夹加载乐谱");
            score_list.model=[];
            return;
        }
        reloadScoreLs(score_groups.currentText);
        // var scoreLs = score_man.getScoreNamesByGroup(score_groups.currentText);
        // if(scoreLs.length===0){
        //     log_label.text=qsTr("乐谱组数据列表为空:")+score_groups.currentText;
        //     score_list.model=[];
        //     return;
        // }
        // log_label.text=qsTr("请双击选中对应乐谱进入演奏模式：");
        // score_list.model=scoreLs;
    }
    Component.onCompleted: {
        // score_groups.model = curScoreGroups;
        // console.log("Component.onCompleted")
        resetScoreList();
    }
    // function scoreGroupChangedFun(groupname) {

    // }

    ScoreGroupMan{
        id:score_man
        onScoreGroupChanged:
        {
            resetScoreList();
            if(groupname!=="")
            {

                // console.log("2groupname:",groupname);
                for(var i = 0;i<score_groups.model.length;++i)
                {
                    var onename = score_groups.model[i];
                    if(onename===groupname)
                    {
                        score_groups.currentIndex = i;
                        // console.log("i match:",i);
                    }
                }
                // console.log("groupname:",groupname);

                // score_groups.model=groupname;
                // score_groups.currentText = groupname;
                reloadScoreLs(groupname);
            }

        }
    }

    Settings {
        id: settings
        property string style
    }

    Shortcut {
        sequences: ["Esc", "Back"]
        enabled: stackView.depth > 1
        onActivated: navigateBackAction.trigger()
    }

    Shortcut {
        sequence: StandardKey.HelpContents
        onActivated: window.help()
    }

    Action {
        id: navigateBackAction
        // icon.name: stackView.depth > 1 ? "back" : "drawer"
        icon.source: stackView.depth > 1 ? "qrc:/images/back.svg" : "qrc:/images/note.svg"
        onTriggered: {
            if (stackView.depth > 1) {
                stackView.pop()
                // listView.currentIndex = -1
                titleLabel.text="PtePlayer";
            } else {
                // drawer.open()
            }
        }
    }

    Shortcut {
        sequence: "Menu"
        onActivated: optionsMenuAction.trigger()
    }

    Action {
        id: optionsMenuAction
        icon.name: "menu"
        onTriggered: optionsMenu.open()
    }
    // function scoreNameChanged(name: string) {
    //     console.log("scoreNameChanged aa ",name)
    // }

    header: App.ToolBar {
        id:main_head
        RowLayout {
            spacing: 20
            anchors.fill: parent
            // anchors.leftMargin: !window.portraitMode ? drawer.width : undefined

            ToolButton {
                action: navigateBackAction
                visible: window.portraitMode
            }

            Label {
                id: titleLabel
                text: qsTr("PtePLayer")
                font.pixelSize: 20
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            ToolButton {
                action: optionsMenuAction

                Menu {
                    id: optionsMenu
                    x: parent.width - width
                    transformOrigin: Menu.TopRight

                    // Action {
                    //     text: qsTr("Settings")
                    //     onTriggered: settingsDialog.open()
                    // }
                    // Action {
                    //     text: qsTr("Help")
                    //     onTriggered: window.help()
                    // }
                    Action {
                        text: qsTr("说明")
                        onTriggered: aboutDialog.open()
                    }
                    // Action {
                    //     text: qsTr("横屏")
                    //     onTriggered: {
                    //         window.contentOrientation = Qt.LandscapeOrientation;
                    //     }
                    // }
                    // Action {
                    //     text: qsTr("竖屏")
                    //     onTriggered: {
                    //         window.contentOrientation = Qt.PortraitOrientation;
                    //     }
                    // }
                }
            }
        }
    }



    StackView {
        id: stackView
        // width: parent.width
        // height: parent.height
        anchors.fill: parent
        // anchors.leftMargin: !window.portraitMode ? drawer.width : undefined
        function stackLargeSize(){
            return stackView.height>stackView.width?stackView.height:stackView.width;
        }
        function stackWHScale(){
            return stackView.width/stackView.height;
        }
        function getHeightScale()
        {
            var basescale = stackWHScale();
            if(basescale>1.2)
            {
                return 1.2;
            }
            if(basescale<0.8){
                return 0.8;
            }
            return basescale;
        }

        initialItem: Pane {
            id: pane
            // width: stackView.width
            // height: stackView.height
            anchors.fill: parent
            Column
            {
                id:init_col_ly
                anchors.fill: parent
                width: stackView.width
                height: stackView.height
                Row
                {
                    id:score_groups_rly
                    height: stackView.height*0.12*stackView.getHeightScale()
                    width: stackView.width
                    // anchors.fill: parent
                    ComboBox {
                        id:score_groups
                        width: stackView.width*0.3
                        height: parent.height
                        onCurrentTextChanged: {
                            reloadScoreLs(currentText);
                            filtertext.text="";
                        }
                        // Layout.width: stackView.width*0.5
                        // model: [qsTr("First"), qsTr("Second"), qsTr("Third")]
                    }
                    RoundButton {
                        // anchors.left: score_groups.right
                        width: parent.height
                        height: parent.height
                        id: add_group_btn
                        text: "+"
                        onClicked:
                        {
                            score_man.newScoreGroup();
                        }
                        Component.onCompleted: {
                            background.opacity=0.7;
                            background.color=main_head.background.color
                            palette.buttonText="royalblue";
                        }
                        contentItem: Text {
                            text: add_group_btn.text
                            font: add_group_btn.font
                            color: "white"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }
                    }
                    RoundButton {

                        width: parent.height
                        height: parent.height
                        // anchors.left: add_group_btn.right
                        id: remove_group_btn
                        text: "-"
                        onClicked: {
                            if(score_groups.count===0){
                                return;
                            }
                            confirmationDelScoreDialog.open();
                        }
                        Component.onCompleted: {
                            background.opacity=0.7;
                            background.color=main_head.background.color
                        }
                        contentItem: Text {
                            text: remove_group_btn.text
                            font: remove_group_btn.font
                            color: "white"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }
                        Dialog {
                            id: confirmationDelScoreDialog

                            x: (parent.width - width) / 2
                            y: (parent.height - height) / 2
                            parent: Overlay.overlay

                            modal: true
                            title: qsTr("提示")
                            standardButtons: Dialog.Yes | Dialog.No

                            Column {
                                spacing: 20
                                anchors.fill: parent
                                Label {
                                    text: qsTr("确定要删除谱组")+score_groups.currentText+"?";
                                }
                            }
                            onAccepted: {
                                score_man.delScoreGroup(score_groups.currentText);
                            }
                        }
                    }
                    TextField {
                        id:filtertext
                        width: stackView.width*0.3
                        height: parent.height
                        placeholderText: "搜索歌曲"
                        onTextEdited:{
                            console.log("filtertext:",filtertext.text);
                            reloadScoreLsFilter(score_groups.currentText,filtertext.text);
                        }
                    }
                    // anchors.horizontalCenter: parent.horizontalCenter
                    // Layout.fillWidth: true
                    // anchors.top: parent.top
                }


                Label {
                    id:log_label
                    text: qsTr("请双击选中对应乐谱进入演奏模式：")
                    // anchors.margins: stackView.height*0.2
                    // anchors.top: score_groups_rly.bottom
                    // anchors.left: parent.left
                    // anchors.right: parent.right
                    // anchors.bottom: sorelstlayout.top
                    horizontalAlignment: Label.AlignLeft
                    verticalAlignment: Label.AlignVCenter
                    // wrapMode: Label.Wrap
                    // Layout.fillHeight: true
                    // Layout.fillWidth: true
                    height: stackView.height*0.10
                    width: stackView.width
                    // anchors.horizontalCenter: parent.horizontalCenter
                }
                // ToolSeparator {
                //     id: toolSeparator
                //     width: stackView.width
                //     height: stackView.height*0.05
                //     bottomPadding: 20
                //     topPadding: 0
                //     leftPadding: 0
                //     rightPadding: 0
                //     orientation: Qt.Horizontal
                // }
                ListView {
                    // visible: false
                    // anchors.horizontalCenter: parent.horizontalCenter
                    Layout.fillHeight: true
                    // Layout.fillWidth: true
                    width: parent.width
                    height: stackView.height-score_groups_rly.height-log_label.height*2
                    // Layout.fillHeight: true
                    // height: stackView.height*0.45
                    id: score_list
                    // focus: true
                    currentIndex: -1
                    // anchors.fill: sorelstlayout
                    // height: parent.height*0.6;
                    // anchors.left: parent.left
                    // anchors.right: parent.right
                    // anchors.bottom: parent.bottom
                    clip: true
                    spacing: 2
                    delegate: ItemDelegate {
                        id: scorels_delegateItem
                        width: ListView.view.width
                        text: modelData
                        // highlighted: ListView.isCurrentItem

                        required property int index
                        required property var modelData
                        onDoubleClicked: {
                            // score_list.currentIndex = index
                            stackView.push("qrc:/pages/ScorePlayer.qml",
                                           {
                                               "score_name":modelData,
                                               "score_group":score_groups.currentText,
                                               "score_list":score_man.getScoreNamesByGroup(score_groups.currentText)
                                           });
                            // scoreplayer.scoreNameChange.connect(scoreNameChanged);
                        }

                        Component.onCompleted: {
                            if(index%2!==0)
                            {
                                background.opacity=0.5;
                                background.color=main_head.background.color;
                            }else
                            {
                                background.opacity=0.3;
                                background.color=main_head.background.color;
                            }

                            // background.inset=5;
                            // background.padding=10;
                        }
                        // onClicked: {
                        //     listView.currentIndex = index
                        //     stackView.push(source)
                        //     if (window.portraitMode)
                        //         drawer.close()
                        // }
                    }

                    // ScrollIndicator.vertical: ScrollIndicator { }
                }

            }


            Image {
                id: main_icon
                source: "qrc:/icons/r_app_icon.svg"
                anchors.right:  parent.right
                anchors.bottom:  parent.bottom
                visible: true
                height:stackView.stackLargeSize()*0.2
                width: stackView.stackLargeSize()*0.2
                opacity: 0.5
            }

            // Image {
            //     id: arrow
            //     source: "images/arrow.png"
            //     anchors.left: parent.left
            //     anchors.bottom: parent.bottom
            //     visible: window.portraitMode
            // }
        }
    }

    Dialog {
        id: settingsDialog
        x: Math.round((window.width - width) / 2)
        y: Math.round(window.height / 6)
        width: Math.round(Math.min(window.width, window.height) / 3 * 2)
        modal: true
        focus: true
        title: qsTr("Settings")

        standardButtons: Dialog.Ok | Dialog.Cancel
        onAccepted: {
            settings.style = styleBox.displayText
            settingsDialog.close()
        }
        onRejected: {
            styleBox.currentIndex = styleBox.styleIndex
            settingsDialog.close()
        }

        contentItem: ColumnLayout {
            id: settingsColumn
            spacing: 20

            RowLayout {
                spacing: 10

                Label {
                    text: qsTr("Style:")
                }

                ComboBox {
                    id: styleBox
                    property int styleIndex: -1
                    model: window.builtInStyles
                    Component.onCompleted: {
                        styleIndex = find(settings.style, Qt.MatchFixedString)
                        if (styleIndex !== -1)
                            currentIndex = styleIndex
                    }
                    Layout.fillWidth: true
                }
            }

            CheckBox {
                id: orientationCheckBox
                text: qsTr("Enable Landscape")
                checked: false
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("Restart required")
                color: "#e41e25"
                opacity: styleBox.currentIndex !== styleBox.styleIndex ? 1.0 : 0.0
                horizontalAlignment: Label.AlignHCenter
                verticalAlignment: Label.AlignVCenter
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    Dialog {
        id: aboutDialog
        modal: true
        focus: true
        title: qsTr("程序说明")
        x: (window.width - width) / 2
        y: window.height / 6
        width: Math.min(window.width, window.height) / 3 * 2
        contentHeight: aboutColumn.height

        Column {
            id: aboutColumn
            spacing: 10

            Label {
                width: aboutDialog.availableWidth
                text: qsTr("该程序为个人开源软件，禁止用于非法盈利！")
                wrapMode: Label.Wrap
                font.pixelSize: 12
            }

            Label {
                width: aboutDialog.availableWidth
                text: qsTr("目前唯一更新渠道为微信公众号：<b>SingLYX</b> 中文名称：<b>平静的爆炸头</b> "
                            +"可添加微信公众号查看相关教程并获得吉他谱子!")
                wrapMode: Label.Wrap
                font.pixelSize: 12
            }
            Label {
                width: aboutDialog.availableWidth
                text: qsTr("该软件还没有上架到个平台应用商店，其他渠道安装导致一切问题不负相关责任")
                wrapMode: Label.Wrap
                font.pixelSize: 12
            }
            RoundButton {
                anchors.horizontalCenter: parent.horizontalCenter
                // anchors.left: score_groups.right
                // width: parent.height
                // height: parent.height
                id: help_btn
                text: "点击跳转到开源网址!"
                onClicked:
                {
                    Qt.openUrlExternally("https://github.com/SinigLi/pteplayer/tree/main");
                }
            }
        }
    }
}
