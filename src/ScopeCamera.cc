#include "ScopeCamera.h"

#include <QtAndroidExtras/QtAndroid>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QAndroidJniEnvironment>

#include <QDebug>

//    This is for the function of Scope the target and place it on Map
//    Made by Geolone
//    You can contact by E-mail:gaojialong@nudt.edu.cn

ScopeCamera::ScopeCamera(QObject *parent)
{
    Q_UNUSED(parent);
//add sensor
//    _internalGPS = QGeoPositionInfoSource::createDefaultSource(this);
//    _internalGPS->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
//    _internalGPS->setUpdateInterval(_updateInterval);
//    connect(_internalGPS, &QGeoPositionInfoSource::positionUpdated,       this, &ScopeCamera::_positionUpdated);
//    connect(_internalGPS, SIGNAL(error(QGeoPositionInfoSource::Error)),   this, SLOT(_error(ScopeCamera::Error)));
//    _internalGPS->startUpdates();
}
ScopeCamera::~ScopeCamera()
{
    _internalGPS->stopUpdates();
}
QObject *ScopeCamera::getQmlCamera() const
{
    return _qmlCamera;
}

void ScopeCamera::setQmlCamera(QObject *qmlCamera)
{
    this->_qmlCamera = qmlCamera;
    qDebug()<<"Geo:You successfully toggle the qmlCamera";
    if(_qmlCamera)
    {
        _camera = qvariant_cast<QCamera*>(_qmlCamera->property("mediaObject"));

        // 这里修改采集格式貌似也没有用呢
        //QCameraViewfinderSettings cameraSettings = _camera->viewfinderSettings();
        //cameraSettings.setPixelFormat(QVideoFrame::Format_YUYV);
        //_camera->setViewfinderSettings(cameraSettings);
        qDebug() << "CameraModel description :" << qvariant_cast<QString>(_camera->metaData("CameraModel"));
        if(_probe.setSource(_camera))
            connect(&_probe, SIGNAL(videoFrameProbed(const QVideoFrame &)), this, SLOT(_onProcssFram(const QVideoFrame &)));
    }
}

void ScopeCamera::_onProcssFram(const QVideoFrame &frame)
{
    if(!frame.isValid())
    {
        return;
    }
    cloneFrame = frame;
}

void ScopeCamera::NV21_TO_RGB24(unsigned char *data, unsigned char *rgb, int width, int height)
{
    int index = 0;

    unsigned char *ybase = data;
    unsigned char *ubase = &data[width * height];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            //YYYYYYYYVUVU
            u_char Y = ybase[x + y * width];
            u_char U = ubase[y / 2 * width + (x / 2) * 2 + 1];
            u_char V = ubase[y / 2 * width + (x / 2) * 2];

            rgb[index++] = Y + 1.402 * (V - 128); //R
            rgb[index++] = Y - 0.34413 * (U - 128) - 0.71414 * (V - 128); //G
            rgb[index++] = Y + 1.772 * (U - 128); //B
        }
    }
}

QString *ScopeCamera::getQmlSensor() const
{
    return _sensorInfo;
}
QObject *ScopeCamera::getQmlScope() const
{
    return _qmlScope;
}
//void ScopeCamera::setQmlGCS(QGeoCoordinate *qmlPostion){
//    qDebug()<<"lat:"<<qmlPostion->latitude()<<"lon:"<<qmlPostion->longitude()<<"alt:"<<qmlPostion->altitude();
//    _qmlPostion = qmlPostion;
//}
void ScopeCamera::setQmlScope(QObject *qmlScope)
{
    this->_qmlScope = qmlScope;
    qDebug()<<"Geo:You successfully toggle the qmlScope";
    if(_qmlScope)
    {
       cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
        QImage recvImage( cloneFrame.size(), QImage::Format_ARGB32);
        if (cloneFrame.pixelFormat()==QVideoFrame::Format_NV21)
        {
            NV21_TO_RGB24(cloneFrame.bits(), recvImage.bits(), cloneFrame.width(), cloneFrame.height());
        }
        else
        {
            QImage recvImage(cloneFrame.bits(), cloneFrame.width(), cloneFrame.height(), QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat()));
        }
        qDebug() << recvImage.format()<< "Image width : " << recvImage.width() << " height : " << recvImage.height();
        QString filepath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/last.jpg";

        qDebug()<<"path:"<<filepath<<"saved:"<<recvImage.save(filepath,"JPG",100)<<"set permission"<<QFile::setPermissions(filepath,QFileDevice::ExeOther);

        cloneFrame.unmap();
    }
//    if(_gcs_isvalid){
//        qDebug()<<"lat:"<<_gcsPosition.latitude()<<"lon:"<<_gcsPosition.longitude()<<"alt:"<<_gcsPosition.altitude();
//    }
}
//sensor update
//void ScopeCamera::_positionUpdated(const QGeoPositionInfo &update)
//{
////    _geoPositionInfo = update;
////    _gcsHeading = update.attribute(QGeoPositionInfo::Direction);

////    if (update.isValid()) {
////        // Note that gcsPosition filters out possible crap values
////       _gcsPosition = update.coordinate();
////       _gcs_isvalid = true;
////    }
//}
//void ScopeCamera::_error(QGeoPositionInfoSource::Error positioningError)
//{
////    qWarning() << "QGCPositionManager error" << positioningError;
//}
