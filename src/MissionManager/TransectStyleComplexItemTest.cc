/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "TransectStyleComplexItemTest.h"
#include "QGCApplication.h"

TransectStyleComplexItemTest::TransectStyleComplexItemTest(void)
{
}

void TransectStyleComplexItemTest::init(void)
{
    TransectStyleComplexItemTestBase::init();

    _transectStyleItem = new TestTransectStyleItem(_masterController, this);
    _transectStyleItem->cameraTriggerInTurnAround()->setRawValue(false);
    _transectStyleItem->cameraCalc()->setCameraBrand(CameraCalc::canonicalCustomCameraName());
    _transectStyleItem->cameraCalc()->valueSetIsDistance()->setRawValue(true);
    _transectStyleItem->cameraCalc()->distanceToSurface()->setRawValue(100);
    _transectStyleItem->setDirty(false);

    _rgSignals[cameraShotsChangedIndex] =               SIGNAL(cameraShotsChanged());
    _rgSignals[timeBetweenShotsChangedIndex] =          SIGNAL(timeBetweenShotsChanged());
    _rgSignals[visualTransectPointsChangedIndex] =      SIGNAL(visualTransectPointsChanged());
    _rgSignals[coveredAreaChangedIndex] =               SIGNAL(coveredAreaChanged());
    _rgSignals[dirtyChangedIndex] =                     SIGNAL(dirtyChanged(bool));
    _rgSignals[complexDistanceChangedIndex] =           SIGNAL(complexDistanceChanged());
    _rgSignals[greatestDistanceToChangedIndex] =        SIGNAL(greatestDistanceToChanged());
    _rgSignals[additionalTimeDelayChangedIndex] =       SIGNAL(additionalTimeDelayChanged());
    _rgSignals[lastSequenceNumberChangedIndex] =        SIGNAL(lastSequenceNumberChanged(int));

    _multiSpy = new MultiSignalSpy();
    QCOMPARE(_multiSpy->init(_transectStyleItem, _rgSignals, _cSignals), true);
}

void TransectStyleComplexItemTest::cleanup(void)
{
    delete _transectStyleItem;
    delete _multiSpy;
    TransectStyleComplexItemTestBase::cleanup();
}

void TransectStyleComplexItemTest::_testDirty(void)
{
    QVERIFY(!_transectStyleItem->dirty());
    _transectStyleItem->setDirty(false);
    QVERIFY(!_transectStyleItem->dirty());
    QVERIFY(_multiSpy->checkNoSignals());

    _transectStyleItem->setDirty(true);
    QVERIFY(_transectStyleItem->dirty());
    QVERIFY(_multiSpy->checkOnlySignalByMask(dirtyChangedMask));
    QVERIFY(_multiSpy->pullBoolFromSignalIndex(dirtyChangedIndex));
    _multiSpy->clearAllSignals();

    _transectStyleItem->setDirty(false);
    QVERIFY(!_transectStyleItem->dirty());
    QVERIFY(_multiSpy->checkOnlySignalByMask(dirtyChangedMask));
    _multiSpy->clearAllSignals();

    // These facts should set dirty when changed
    QList<Fact*> rgFacts;
    rgFacts << _transectStyleItem->turnAroundDistance()
            << _transectStyleItem->cameraTriggerInTurnAround()
            << _transectStyleItem->hoverAndCapture()
            << _transectStyleItem->refly90Degrees();
    for(Fact* fact: rgFacts) {
        qDebug() << fact->name();
        QVERIFY(!_transectStyleItem->dirty());
        changeFactValue(fact);
        QVERIFY(_multiSpy->checkSignalByMask(dirtyChangedMask));
        _transectStyleItem->setDirty(false);
        _multiSpy->clearAllSignals();
    }
    rgFacts.clear();

    _transectStyleItem->adjustSurveAreaPolygon();
    QVERIFY(_transectStyleItem->dirty());
    _transectStyleItem->setDirty(false);
    QVERIFY(!_transectStyleItem->surveyAreaPolygon()->dirty());
    _multiSpy->clearAllSignals();

    changeFactValue(_transectStyleItem->cameraCalc()->distanceToSurface());
    QVERIFY(_transectStyleItem->dirty());
    _transectStyleItem->setDirty(false);
    QVERIFY(!_transectStyleItem->cameraCalc()->dirty());
    _multiSpy->clearAllSignals();
}

void TransectStyleComplexItemTest::_testRebuildTransects(void)
{
    // Changing the survey polygon should trigger:
    //  _rebuildTransects calls
    //  coveredAreaChanged signal
    //  lastSequenceNumberChanged signal
    _transectStyleItem->adjustSurveAreaPolygon();
    QVERIFY(_transectStyleItem->rebuildTransectsPhase1Called);
    QVERIFY(_transectStyleItem->recalcCameraShotsCalled);
    // FIXME: Temproarily not possible
    //QVERIFY(_transectStyleItem->recalcComplexDistanceCalled);
    QVERIFY(_multiSpy->checkSignalsByMask(coveredAreaChangedMask | lastSequenceNumberChangedMask));
    _transectStyleItem->rebuildTransectsPhase1Called = false;
    _transectStyleItem->recalcCameraShotsCalled = false;
    _transectStyleItem->recalcComplexDistanceCalled = false;
    _transectStyleItem->setDirty(false);
    _multiSpy->clearAllSignals();

    // Changes to these facts should trigger:
    //  _rebuildTransects calls
    //  lastSequenceNumberChanged signal
    QList<Fact*> rgFacts;
    rgFacts << _transectStyleItem->turnAroundDistance()
            << _transectStyleItem->cameraTriggerInTurnAround()
            << _transectStyleItem->hoverAndCapture()
            << _transectStyleItem->refly90Degrees()
            << _transectStyleItem->cameraCalc()->frontalOverlap()
            << _transectStyleItem->cameraCalc()->sideOverlap();
    for(Fact* fact: rgFacts) {
        qDebug() << fact->name();
        changeFactValue(fact);
        QVERIFY(_transectStyleItem->rebuildTransectsPhase1Called);
        QVERIFY(_transectStyleItem->recalcCameraShotsCalled);
        // FIXME: Temproarily not possible
        //QVERIFY(_transectStyleItem->recalcComplexDistanceCalled);
        QVERIFY(_multiSpy->checkSignalsByMask(lastSequenceNumberChangedMask));
        _transectStyleItem->setDirty(false);
        _multiSpy->clearAllSignals();
        _transectStyleItem->rebuildTransectsPhase1Called = false;
        _transectStyleItem->recalcCameraShotsCalled = false;
        _transectStyleItem->recalcComplexDistanceCalled = false;
    }
    rgFacts.clear();

    _transectStyleItem->cameraCalc()->valueSetIsDistance()->setRawValue(false);
    _transectStyleItem->rebuildTransectsPhase1Called = false;
    _transectStyleItem->recalcCameraShotsCalled = false;
    _transectStyleItem->recalcComplexDistanceCalled = false;
    changeFactValue(_transectStyleItem->cameraCalc()->imageDensity());
    QVERIFY(_transectStyleItem->rebuildTransectsPhase1Called);
    QVERIFY(_transectStyleItem->recalcCameraShotsCalled);
    // FIXME: Temproarily not possible
    //QVERIFY(_transectStyleItem->recalcComplexDistanceCalled);
    QVERIFY(_multiSpy->checkSignalsByMask(lastSequenceNumberChangedMask));
    _multiSpy->clearAllSignals();

    _transectStyleItem->cameraCalc()->valueSetIsDistance()->setRawValue(true);
    _transectStyleItem->rebuildTransectsPhase1Called = false;
    _transectStyleItem->recalcCameraShotsCalled = false;
    _transectStyleItem->recalcComplexDistanceCalled = false;
    changeFactValue(_transectStyleItem->cameraCalc()->distanceToSurface());
    QVERIFY(_transectStyleItem->rebuildTransectsPhase1Called);
    QVERIFY(_transectStyleItem->recalcCameraShotsCalled);
    // FIXME: Temproarily not possible
    //QVERIFY(_transectStyleItem->recalcComplexDistanceCalled);
    QVERIFY(_multiSpy->checkSignalsByMask(lastSequenceNumberChangedMask));
    _multiSpy->clearAllSignals();
}

void TransectStyleComplexItemTest::_testDistanceSignalling(void)
{
    _transectStyleItem->adjustSurveAreaPolygon();
    QVERIFY(_multiSpy->checkSignalsByMask(complexDistanceChangedMask | greatestDistanceToChangedMask));
    _transectStyleItem->setDirty(false);
    _multiSpy->clearAllSignals();

    QList<Fact*> rgFacts;
    rgFacts << _transectStyleItem->turnAroundDistance()
            << _transectStyleItem->hoverAndCapture()
            << _transectStyleItem->refly90Degrees();
    for(Fact* fact: rgFacts) {
        qDebug() << fact->name();
        changeFactValue(fact);
        QVERIFY(_multiSpy->checkSignalsByMask(complexDistanceChangedMask | greatestDistanceToChangedMask));
        _transectStyleItem->setDirty(false);
        _multiSpy->clearAllSignals();
    }
    rgFacts.clear();
}



void TransectStyleComplexItemTest::_testAltMode(void)
{
    // Default should be relative
    QVERIFY(_transectStyleItem->cameraCalc()->distanceToSurfaceRelative());

    // Manual camera allows non-relative altitudes, validate that changing back to known
    // camera switches back to relative
    _transectStyleItem->cameraCalc()->setCameraBrand(CameraCalc::canonicalManualCameraName());
    _transectStyleItem->cameraCalc()->setDistanceToSurfaceRelative(false);
    _transectStyleItem->cameraCalc()->setCameraBrand(CameraCalc::canonicalCustomCameraName());
    QVERIFY(_transectStyleItem->cameraCalc()->distanceToSurfaceRelative());

    // When you turn off terrain following mode make sure that the altitude mode changed back to relative altitudes
    _transectStyleItem->cameraCalc()->setDistanceToSurfaceRelative(false);
    _transectStyleItem->setFollowTerrain(true);

    QVERIFY(!_transectStyleItem->cameraCalc()->distanceToSurfaceRelative());
    QVERIFY(_transectStyleItem->followTerrain());

    _transectStyleItem->setFollowTerrain(false);
    QVERIFY(_transectStyleItem->cameraCalc()->distanceToSurfaceRelative());
    QVERIFY(!_transectStyleItem->followTerrain());
}

void TransectStyleComplexItemTest::_testFollowTerrain(void) {
    _transectStyleItem->cameraCalc()->distanceToSurface()->setRawValue(50);
    _transectStyleItem->cameraCalc()->adjustedFootprintFrontal()->setRawValue(0);
    _transectStyleItem->setFollowTerrain(true);

    QVERIFY(QTest::qWaitFor([&]() { return _transectStyleItem->readyForSaveState() == TransectStyleComplexItem::ReadyForSave; }, 2000));

    QList<MissionItem*> rgItems;
    _transectStyleItem->appendMissionItems(rgItems, this);

    QList<double> expectedTerrainValues {497, 509, 512, 512 };
    //QCOMPARE(rgItems.count(), expectedTerrainValues.count());
    for (const MissionItem* missionItem : rgItems) {
        QCOMPARE(missionItem->command(), MAV_CMD_NAV_WAYPOINT);
        QCOMPARE(missionItem->frame(), MAV_FRAME_GLOBAL);
        QCOMPARE(missionItem->param7(), expectedTerrainValues.front());
        expectedTerrainValues.pop_front();
    }
}

TestTransectStyleItem::TestTransectStyleItem(PlanMasterController* masterController, QObject* parent)
    : TransectStyleComplexItem      (masterController, false /* flyView */, QStringLiteral("UnitTestTransect"), parent)
    , rebuildTransectsPhase1Called  (false)
    , recalcComplexDistanceCalled   (false)
    , recalcCameraShotsCalled       (false)
{
    // We use a 100m by 100m square test polygon
    const double edgeDistance = 100;
    surveyAreaPolygon()->appendVertex(UnitTestTerrainQuery::linearSlopeRegion.center());
    surveyAreaPolygon()->appendVertex(surveyAreaPolygon()->vertexCoordinate(0).atDistanceAndAzimuth(edgeDistance, 90));
    surveyAreaPolygon()->appendVertex(surveyAreaPolygon()->vertexCoordinate(1).atDistanceAndAzimuth(edgeDistance, 180));
    surveyAreaPolygon()->appendVertex(surveyAreaPolygon()->vertexCoordinate(2).atDistanceAndAzimuth(edgeDistance, -90.0));
}

void TestTransectStyleItem::_rebuildTransectsPhase1(void)
{
    rebuildTransectsPhase1Called = true;

    _transects.clear();
    if (_surveyAreaPolygon.count() < 3) {
        return;
    }

    _transects.append(QList<TransectStyleComplexItem::CoordInfo_t>{
        {surveyAreaPolygon()->vertexCoordinate(0), CoordTypeSurveyEntry},
        {surveyAreaPolygon()->vertexCoordinate(2), CoordTypeSurveyExit}}
    );
}

void TestTransectStyleItem::_recalcCameraShots(void)
{
    recalcCameraShotsCalled = true;
}

void TestTransectStyleItem::adjustSurveAreaPolygon(void)
{
    QGeoCoordinate vertex = surveyAreaPolygon()->vertexCoordinate(0);
    vertex.setLatitude(vertex.latitude() + 1);
    surveyAreaPolygon()->adjustVertex(0, vertex);
}

