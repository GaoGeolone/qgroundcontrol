#ifndef SCOPECAMERA_H
#define SCOPECAMERA_H
//    This is for the function of Scope the target and place it on Map
//    Made by Geolone
//    You can contact by E-mail:gaojialong@nudt.edu.cn
#include <QObject>
#include <QVideoProbe>
#include <QVideoFrame>
#include <QCamera>
// Geo : add android sensors
// Main Classed
#include <QSensor>
#include <QSensorFilter>
#include <QSensorReading>
// Reading classes
// Gyro
#include <QGyroscope>
#include <QGyroscopeFilter>
#include <QAccelerometerReading>
// compass
#include <QCompass>
#include <QCompassFilter>
#include <QCompassReading>


#include <QGeoPositionInfoSource>
#include <QGeoCoordinate>
#include <QGeoPositionInfo>
// add other
#include <QFile>
#include <QStandardPaths>


class ScopeCamera : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject* qmlCamera READ getQmlCamera WRITE setQmlCamera)
    Q_PROPERTY(QObject* qmlScope READ getQmlScope WRITE setQmlScope)
//    Q_PROPERTY(QGeoCoordinate* qmlPostion WRITE setQmlGCS)
    Q_PROPERTY(QString* qmlSensor READ getQmlSensor)
public:
    explicit ScopeCamera(QObject *parent = nullptr);
    ~ScopeCamera();
    //process the Picture by stream(for test)
    QObject *getQmlCamera() const;
    void setQmlCamera(QObject *qmlCamera);
    // Scope
    QObject *getQmlScope() const;
    void setQmlScope(QObject *qmlScope);
    //toggle the Scope function
    QString *getQmlSensor() const;
    //wriete to the GCS postion  Invalid write to global property "qmlPostion"
//    void setQmlGCS(QGeoCoordinate *qmlPostion);
    //output to qml
    void NV21_TO_RGB24(unsigned char *data, unsigned char *rgb, int width, int height);
//private:
//signals:

private slots:
    void _onProcssFram(const QVideoFrame &);
////sensor
//    void _positionUpdated(const QGeoPositionInfo &update);
//    void _error(QGeoPositionInfoSource::Error positioningError);
private:
    QObject *_qmlCamera = nullptr;
    QCamera *_camera = nullptr;
    QObject *_qmlScope = nullptr;
    QImage *_image = nullptr;
    QVideoProbe _probe;
    QVideoFrame cloneFrame;
    QString *_sensorInfo = nullptr;
// Use Internal GPS source
    int                 _updateInterval =   200;
    QGeoPositionInfo    _geoPositionInfo;
    QGeoCoordinate      _gcsPosition;
    qreal               _gcsHeading =       qQNaN();
    QGeoPositionInfoSource* _internalGPS =        nullptr;
    bool                _gcs_isvalid = false;
};
#endif // SCOPECAMERA_H
