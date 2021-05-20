import QtQuick          2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts  1.2

import QtMultimedia 5.2
import ScopeCamera 1.0

Item {
    id : cameraUI
    anchors.fill:               parent
//    anchors.margins:            ScreenTools.defaultFontPixelWidth
    state: "PhotoCapture"
    Camera {
        id: camera
        captureMode: Camera.CaptureStillImage
        focus{
            focusMode:Camera.FocusAuto;//自动变焦
            focusPointMode: Camera.FocusPointCenter;//中心焦点
        }
        imageCapture {
            onImageCaptured: {
            // Show the preview in an Image
//                 photoPreview.source = preview
             }
            onImageSaved: {
                console.log("image has been saved: " + requestId);
                console.log("saved path: " + path);
                photoPreview.source = path;
            }
        }
    }

    VideoOutput {
        id:viewfinder
        source: camera
//        fillMode: Stretch
        focus : visible // to receive focus and capture key events when visible
        anchors.fill: parent
        autoOrientation: false
        orientation: 0
        MouseArea{
            anchors.fill: parent
            onClicked: {
                camera.searchAndLock();
                scopeprocess.qmlScope=camera
            }
        }
        Rectangle{ //设置属性与父窗体不同，用于对比
            id:hh
            x: parent.x + parent.width/2 - 15 //相对父窗体的坐标
            y: parent.y + parent.height/2 - 15
            width: 30
            height: 30
            color: "transparent"
            border.color: "red";//边框的一些属性
            rotation: 0
       }
        Image {
            id: photoPreview
//            anchors.fill:parent;
            visible: true
        }
    }

    ZoomControl {
                id:zoomControl
                x : 0
                y : 0
                z:3
                width : 100
                height: parent.height

                currentZoom: camera.digitalZoom
                maximumZoom: Math.min(4.0, camera.maximumDigitalZoom)
                onZoomTo: camera.setDigitalZoom(value)
            }
    ScopeCamera{
        id: scopeprocess
        qmlCamera: camera  //Geo: this will toggle the setQmlCamera
    }
}

