import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Scores

Pane {
    id: pane
    required property string score_name
    required property string score_group
    required property var score_list
    property string lastscore
    property string nextscore
    function scoreViewHadChange()
    {
        flickable.contentHeight=score_chart.getHeightByBaseWidth(pane.width,flickable.height);
        //console.log("scoreViewHadChange flickable.contentHeight :",flickable.contentHeight,"pand width:",pane.width," pane height ",pane.height);
        flickable.update();
        score_chart.update();
    }
    function scroolToCurPos()
    {
        var scenePos= score_chart.getCurPosPosition();
        // var midViewPos = (flickable.height.toPrecision(2) / flickable.contentHeight) *0.5;
        var pos = scenePos ;//+ midViewPos;
        //console.log("pos ",pos," spos ",scenePos);
        if(pos<0){
            pos = 0;
        }else if(pos>1.0){
            pos = 1.0;
        }

        score_scroller.position = pos;
        score_chart.update();
    }

    // signal scoreStartPlay()
    // signal scoreStopPay()
    // signal scoreNameChange(string name);
    function endPalyerBack(){
        startStopPlayer(false);
        // score_scroller.position = 0.5;
    }
    function startStopPlayer(bSynplay)
    {
        if(start_stop_play_btn.bStartPlay)
        {
            start_stop_play_btn.icon.source = "qrc:/images/startPlay.svg";
        }else
        {
            start_stop_play_btn.icon.source = "qrc:/images/stopPlay.svg";
        }
        start_stop_play_btn.bStartPlay = !start_stop_play_btn.bStartPlay;
        if(start_stop_play_btn.bStartPlay){
            main_head.visible = false;
            score_set_ctl.visible = false;
        }else{
            main_head.visible = true;
            score_set_ctl.visible = true;
        }
        if(bSynplay){
            score_chart.startStopPlay();
        }
        if(!start_stop_play_btn.bStartPlay){
            scroolToCurPos();
        }
    }
    function resetToStartPos()
    {
        if(start_stop_play_btn.bStartPlay){
            startStopPlayer(true);
        }
        score_chart.resetToStartPos();
        scroolToCurPos();
    }
    function setPlayerSetInfoLs(setls)
    {
        playerSetList_model.clear();
        for(var i = 0;i<setls.length;++i)
        {
            var onerow = setls[i];
            // console.log("one row \r\n",onerow);
            playerSetList_model.append({"name":onerow["name"],
                                      "tab_stuff_show":onerow["tab_stuff_show"],
                                      "std_stuff_show":onerow["std_stuff_show"],
                                      "number_stuff_show":onerow["number_stuff_show"],
                                      "activated":onerow["activated"],
                                      });
        }
    }

    function openScore(name)
    {
        score_name = name;
        var r = score_chart.openScore(score_group,score_name);
        if(r.length!==0)
        {
            return;
        }
        score_chart.caculateSysSizeByHeight(pane.width,pane.height);
        lastscore = "";
        nextscore = "";
        for(var i=0;i<score_list.length;++i)
        {
            if(score_list[i]===name)
            {
                if(i!=0){
                    lastscore = score_list[i-1];
                }
                if(i!=score_list.length-1){
                    nextscore = score_list[i+1];
                }
            }
        }

        // flickable.contentHeight = score_chart.getHeightByBaseWidth(pane.width,pane.height);
        if(lastscore!="")
        {
            last_score_btn.text="上一首:"+lastscore;
        }else{
            last_score_btn.text="上一首:无";
        }
        if(nextscore!="")
        {
            next_score_btn.text="下一首:"+nextscore;
        }else{
            next_score_btn.text="下一首:无";
        }

        // console.log("score ls  ",score_list);
        // pane.update();
        // score_chart.update();
        start_bar_box.value = score_chart.startBarNum();
        start_bar_box.to = score_chart.allBarCount();
        bp_rate_box.value = score_chart.curBpRate();
        // start_bar_box.to = score_chart.allBarCount();
        var setls = score_chart.curPlayerSetting();
        setPlayerSetInfoLs(setls);

        titleLabel.text = name+" ("+bp_rate_box.value+"bp)";
    }

    Component.onCompleted: {
        listView_scorels.model = score_list;
        openScore(score_name);

    }
    onHeightChanged:
    {
        // resetToStartPos();
        score_chart.caculateSysSizeByHeight(pane.width,pane.height);
    }
    onWidthChanged:
    {
        resetToStartPos();
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        contentHeight: score_chart.getHeightByBaseWidth(pane.width,pane.height);

        ScoreChart{
            id:score_chart

            anchors.fill: parent

            onViewHadChanged:scoreViewHadChange();
            onEndPlayBack:endPalyerBack();
        }

        ScrollBar.vertical: ScrollBar {
            id:score_scroller
        }
    }
    Row
    {
        id:player_ctl
        function baseHeight()
        {
            var size = pane.height>pane.width?pane.height:pane.width;
            return size *0.1;
        }

        // height: pane.height*0.2
        // width: pane.width*0.2
        anchors.bottom: flickable.bottom
        anchors.bottomMargin: 8
        anchors.horizontalCenter: flickable.horizontalCenter
        RoundButton {
            // anchors.left: score_groups.right
            width: player_ctl.baseHeight()
            height: player_ctl.baseHeight()
            id: restart_btn
            // background: Rectangle
            // {
            //     opacity:0.8
            // }
            icon{
                height:parent.height*0.5
                width:parent.width*0.5
                // color: "blue"
                color: "white"
                source: "qrc:/images/revertPlay.svg"
                // source: "image://standardicons/SP_MediaSkipBackward"
            }
            Component.onCompleted: {
                background.opacity=0.7;
                background.color=main_head.background.color
            }

            onClicked:
            {
                // score_man.newScoreGroup();
                resetToStartPos();
            }
        }
        RoundButton {
            // anchors.left: score_groups.right
            width: player_ctl.baseHeight()
            height: player_ctl.baseHeight()
            id: start_stop_play_btn
            property bool bStartPlay: false
            // text: ">"
            icon{
                // id:pl_ic
                // color: "blue"
                color: "white"
                height:parent.height*0.5
                width:parent.width*0.5
                source: "qrc:/images/startPlay.svg"
            }
            Component.onCompleted: {
                background.opacity=0.7;
                background.color=main_head.background.color
            }
            onClicked:
            {
                startStopPlayer(true);

                // if(pl_ic.source==="image://standardicons/SP_MediaPlay")
                // {
                //     pl_ic.source="image://standardicons/SP_MediaStop"
                // }else{
                //     pl_ic.source="image://standardicons/SP_MediaPlay"
                // }

                // score_man.newScoreGroup();
            }
        }
    }
    Column
    {
        id:score_set_ctl
        anchors.right: flickable.right
        anchors.rightMargin: 8
        anchors.verticalCenter: flickable.verticalCenter
        property int iconheight: player_ctl.baseHeight()*0.8
        Row
        {

            RoundButton {
                // anchors.left: score_groups.right
                width: score_set_ctl.iconheight
                height: score_set_ctl.iconheight
                id: score_set
                // background: Rectangle
                // {
                //     opacity:0.8
                // }
                icon{
                    height:parent.height*0.6
                    width:parent.width*0.6
                    color: "white"
                    // color: "blue"
                    source: "qrc:/images/score_setting.svg"
                    // source: "image://standardicons/SP_MediaSkipBackward"
                }
                Component.onCompleted: {
                    background.opacity=0.7;
                    background.color=main_head.background.color
                }
                onClicked:
                {
                    contentDialog.open();
                    // score_man.newScoreGroup();
                }
            }
            RoundButton {
                // anchors.left: score_groups.right
                width: score_set_ctl.iconheight
                height: score_set_ctl.iconheight
                id: score_list_btn
                // background: Rectangle
                // {
                //     opacity:0.8
                // }
                icon{
                    height:parent.height*0.6
                    width:parent.width*0.6
                    color: "white"
                    source: "qrc:/images/score_list.svg"
                    // source: "image://standardicons/SP_MediaSkipBackward"
                }
                Component.onCompleted: {
                    background.opacity=0.7;
                    background.color=main_head.background.color
                }
                onClicked:
                {
                    drawer_scorels.open();
                    // score_man.newScoreGroup();
                }
            }
        }
        Button
        {
            id: last_score_btn
            width: pane.width*0.3
            height: score_set_ctl.iconheight * 0.8
            text: "上一首:"
            onClicked: {
                if(lastscore!=""){
                    openScore(lastscore);
                }
            }

            Component.onCompleted: {
                background.opacity=0.7;
                background.color=main_head.background.color
            }
            contentItem: Text {
                text: last_score_btn.text
                font: last_score_btn.font
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
        }
        Button
        {
            id: next_score_btn
            width: pane.width*0.3
            height: score_set_ctl.iconheight * 0.8
            text: "下一首:"
            onClicked: {
                if(nextscore!=""){
                    openScore(nextscore);
                }
            }

            Component.onCompleted: {
                background.opacity=0.7;
                background.color=main_head.background.color
            }
            contentItem: Text {
                text: next_score_btn.text
                font: next_score_btn.font
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
        }
    }


    Drawer {
        id: drawer_scorels

        width: Math.min(window.width, window.height) / 3 * 2
        height: window.height
        // modal: true
        // interactive: window.portraitMode ? (stackView.depth === 1) : false
        position: 0
        visible: false

        ListView {
            id: listView_scorels

            focus: true
            // currentIndex: -1
            anchors.fill: parent


            delegate: ItemDelegate {
                id: delegateItem_scorels
                width: ListView.view.width
                text: modelData
                // highlighted: ListView.isCurrentItem

                required property int index
                required property var modelData

                onClicked: {
                    openScore(modelData);
                    // listView.currentIndex = index
                    // stackView.push(source)
                    // if (window.portraitMode)
                    //     drawer.close()
                }
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }
    Dialog {
        id: contentDialog
        function whScale()
        {
            return pane.width/pane.height;
        }
        function getBaseWidth()
        {
            var whscale = whScale();
            if(whscale>1.2)
            {
                return pane.width * 0.5;
            }
            if(whscale<0.8)
            {
                return pane.width * 0.9;
            }
            return pane.width * 0.7;
        }

        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        width: contentDialog.getBaseWidth()
        height: pane.height *0.9;
        // contentHeight: logo.height * 2
        parent: Overlay.overlay
        // implicitFooterHeight: pane.height *0.05;
        modal: true
        title: qsTr("乐谱设置")
        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted:{
            score_chart.setBpRate(bp_rate_box.value);
            score_chart.setStartBar(start_bar_box.value);
            
            var playerSetLs = [];
            for(var i = 0;i<playerSetList_model.count;++i)
            {
                var onerow = playerSetList_model.get(i);
                console.log("one row \r\n",onerow);
                var maprow = {};
                maprow["name"]=onerow["name"];
                maprow["tab_stuff_show"]=onerow["tab_stuff_show"];
                maprow["std_stuff_show"]=onerow["std_stuff_show"];
                maprow["number_stuff_show"]=onerow["number_stuff_show"];
                maprow["activated"]=onerow["activated"];
                playerSetLs.push(maprow);
            }

            score_chart.setPlayerSetting(playerSetLs);
            titleLabel.text = score_name+" ("+bp_rate_box.value+"bp)";
            resetToStartPos();
        }

        Flickable {
            id: flickable_dlg
            clip: true
            anchors.fill: parent
            contentHeight: column.height

            Column {
                id: column
                spacing: 10
                width: parent.width

                RowLayout {
                    // width: column.width
                    anchors.left: parent.left
                    anchors.right: parent.right
                    Layout.fillWidth: true
                    // spacing: 10
                    // anchors.horizontalCenter: parent.horizontalCenter;
                    Label {
                        id:start_bar_label
                        text: qsTr("起始小节:")
                        Layout.preferredWidth:  parent.width*0.4
                        // width: column.width*0.4
                        // Layout.fillWidth: 0.4
                        // Layout.alignment: Qt.AlignHCenter
                        // width: column.width*0.6
                        Layout.fillWidth: true
                        // Layout.minimumWidth: bpm_label.width
                        Layout.horizontalStretchFactor: 2
                        // anchors.left: parent.left
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignLeft
                        // anchors.verticalCenter: parent.verticalCenter;
                    }
                    SpinBox {
                        id: start_bar_box
                        // anchors.left: start_bar_label.right
                        value: 1
                        from: 1
                        to:1500
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignLeft
                        // anchors.horizontalCenter: parent.horizontalCenter
                        editable: true
                        height: pane.height * 0.1
                        Layout.fillWidth: true
                        Layout.horizontalStretchFactor: 3

                        // width: column.width*0.4
                    }
                }
                RowLayout {
                    // width: column.width
                    anchors.left: parent.left
                    anchors.right: parent.right
                    Layout.fillWidth: true
                    // spacing: 10

                    // anchors.horizontalCenter: parent.horizontalCenter;
                    Label {
                        text: qsTr("节拍速度(bp):")
                        id:bpm_label
                        Layout.preferredWidth: parent.width*0.4
                        // width: column.width*0.6
                        Layout.fillWidth: true
                        Layout.horizontalStretchFactor: 2
                        // anchors.left: parent.left
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignLeft
                        // Layout.fillWidth: 0.4
                        // anchors.verticalCenter: parent.verticalCenter;
                    }
                    SpinBox {
                        // anchors.left: bpm_label.right
                        id: bp_rate_box
                        value: 120;
                        from: 1
                        to:1500
                        // anchors.horizontalCenter: parent.horizontalCenter
                        editable: true
                        height: pane.height * 0.1
                        Layout.horizontalStretchFactor: 3
                        // width: column.width*0.4
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignLeft
                        // Layout.fillWidth: 0.6
                    }
                }
                Label {
                    id:player_label
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: qsTr("音轨显示配置:")
                    // Layout.preferredWidth:  parent.width*0.4
                    // width: column.width*0.4
                    // Layout.fillWidth: 0.4
                    // Layout.alignment: Qt.AlignHCenter
                    // width: column.width*0.6
                    // Layout.fillWidth: true
                    // Layout.minimumWidth: bpm_label.width
                    // Layout.horizontalStretchFactor: 2
                    // anchors.left: parent.left
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignLeft
                    // anchors.verticalCenter: parent.verticalCenter;
                }
                ListView {
                    clip: true
                    // anchors.top:player_label.bottom
                    width: parent.width*0.9
                    // anchors.left: parent.left
                    // anchors.right: parent.right
                    height: player_label.height * 6
                    visible: true
                    spacing: 2
                    anchors.left: parent.left
                    // anchors.horizontalCenter: parent.horizontalCenter;
                    id: playerSetListView
                    // Layout.fillWidth: true
                    // height: parent.height*0.2
                    // width: parent.width
                    // Layout.preferredHeight: parent.height *0.6;
                    // Layout.preferredHeight: window.height*0.2
                    // Layout.fillHeight: true
                    // anchors.fill: parent
                    model: PlayerSetModel {
                        id:playerSetList_model
                    }
                    delegate: PlayerSetDelegate {}

                }
                Button
                {
                    // anchors.top:playerSetListView.bottom
                    id:reflesh_set_dlg
                    text: "恢复"

                    anchors.horizontalCenter: parent.horizontalCenter;
                    onClicked:
                    {
                        bp_rate_box.value = score_chart.orgBpRate();
                        start_bar_box.value = 1;
                        var setls = score_chart.orgPlayerSetting();
                        setPlayerSetInfoLs(setls);

                        // playerSetListView.model = score_chart.curPlayerSetting();
                    }
                    Component.onCompleted:
                    {
                        icon.source="qrc:/images/reflesh_set.svg";
                    }
                }
            }

            ScrollIndicator.vertical: ScrollIndicator {
                parent: contentDialog.contentItem
                anchors.top: flickable_dlg.top
                anchors.bottom: flickable_dlg.bottom
                anchors.right: parent.right
                anchors.rightMargin: -contentDialog.rightPadding + 1
            }
        }
    }

}
