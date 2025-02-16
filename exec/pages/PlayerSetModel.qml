// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick

// Populate the model with some sample data.
ListModel {
    id: playerSetModel

    ListElement {
        name: "吉他"
        tab_stuff_show:true
        std_stuff_show:false
        number_stuff_show:false
        activated: true
    }
    ListElement {
        name: "旋律"
        tab_stuff_show:false
        std_stuff_show:false
        number_stuff_show:false
        activated: true
    }
}
