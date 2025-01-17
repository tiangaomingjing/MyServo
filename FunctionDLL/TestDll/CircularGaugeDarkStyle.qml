﻿/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.2
import QtQuick.Controls.Styles 1.4

CircularGaugeStyle {
    id: root
    tickmarkStepSize: 10
    minorTickmarkCount: 5
    labelStepSize: 20
    tickmarkInset: outerRadius * 0.06
    minorTickmarkInset: tickmarkInset
    labelInset: outerRadius * 0.23
    minimumValueAngle: -180;
    maximumValueAngle:180;

    background: Image {
        source: "file:///E:/work/Servo/GTSD/FunctionDLL/TestDll/gaugebackground.png"
    }

    needle: Image {
        id: needleImage
        transformOrigin: Item.Bottom
        source: "file:///E:/work/Servo/GTSD/FunctionDLL/TestDll/gaugeneedle.png"
        scale: {
            var distanceFromLabelToRadius = labelInset / 2;
            var idealHeight = outerRadius - distanceFromLabelToRadius;
            var originalImageHeight = needleImage.sourceSize.height;
            idealHeight / originalImageHeight;
        }
    }

    foreground: Item {
        Image {
            anchors.centerIn: parent
            source: "file:///E:/work/Servo/GTSD/FunctionDLL/TestDll/gaugecenter.png"
            scale: (outerRadius * 0.25) / sourceSize.height
        }
    }

    tickmark: Rectangle {
        implicitWidth: outerRadius * 0.02
        antialiasing: true
        implicitHeight: outerRadius * 0.05
        color: "#888"
    }

    minorTickmark: Rectangle {
        implicitWidth: outerRadius * 0.01
        antialiasing: true
        implicitHeight: outerRadius * 0.02
        color: "#444"
    }

    tickmarkLabel: Text {
        font.pixelSize: Math.max(6, outerRadius * 0.1)
        text: styleData.value
        color: "white"
        visible: styleData.value-360<0?true:false
    }
}
