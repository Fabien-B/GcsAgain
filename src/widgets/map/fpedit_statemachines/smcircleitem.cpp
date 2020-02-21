#include "smcircleitem.h"
#include "mapwidget.h"
#include <QApplication>
#include <QDebug>

SmCircleItem::SmCircleItem(MapWidget* map) :
    FpEditStateMachine (map),
    cir(nullptr), state(FPECSMS_IDLE)
{

}

SmCircleItem::~SmCircleItem() {

}

MapItem* SmCircleItem::update(FPEditEvent event_type, QGraphicsSceneMouseEvent* mouseEvent, WaypointItem* waypoint, QColor color, MapItem* item) {
    (void)item;
    Point2DLatLon latlon(0, 0);
    if(event_type == FPEE_WP_CLICKED) {
        assert(waypoint != nullptr);
    } else {
        assert(mouseEvent != nullptr);
        latlon = latlonPoint(mouseEvent->scenePos(), zoomLevel(map->zoom()), map->tileSize());
    }

    // some variables used later, cause switch are so prehistoric...
    QPointF dp;
    double d;

    switch (state) {
    case FPECSMS_IDLE:
        switch (event_type) {
        case FPEE_SC_PRESS:
            if(mouseEvent->button() == Qt::LeftButton) {
                pressPos = mouseEvent->scenePos();
                cir = new CircleItem(latlon, 0, color, 50, map);
                mouseEvent->accept();
                state = FPECSMS_PRESSED;
                // set moving color ?
                return cir;
            }
            break;
        case FPEE_WP_CLICKED:
            cir = new CircleItem(waypoint, 0, color, 50, map);
            state = FPECSMS_RELEASED;
            qDebug() << "Circle state machine: waypoint clicked!";
            return cir;
        default:
            break;
        }
        break;
    case FPECSMS_PRESSED:
        switch (event_type) {
        case FPEE_SC_MOVE:
            dp = mouseEvent->scenePos()-pressPos;
            d = sqrt(dp.x()*dp.x() + dp.y()*dp.y());
            if(d > qApp->property("MAP_MOVE_HYSTERESIS").toInt()) {
                state = FPECSMS_DRAGING;
            }
            mouseEvent->accept();
            break;
        case FPEE_SC_RELEASE:
            state = FPECSMS_RELEASED;
            mouseEvent->accept();
            break;
        default:
            break;
        }
        break;
    case FPECSMS_DRAGING:
        switch (event_type) {
        case FPEE_SC_MOVE:
            assert(cir != nullptr);
            adjustCircleRadius(mouseEvent);
            mouseEvent->accept();
            break;
        case FPEE_SC_RELEASE:
            // back to normal color ?
            state = FPECSMS_IDLE;
            break;
        default:
            break;
        }
        break;
    case FPECSMS_RELEASED:
        switch (event_type) {
        case FPEE_SC_PRESS:
            adjustCircleRadius(mouseEvent);
            state = FPECSMS_DRAGING;
            mouseEvent->accept();
            break;
        default:
            break;
        }
        break;
    }

    return nullptr;
}


void SmCircleItem::adjustCircleRadius(QGraphicsSceneMouseEvent* mouseEvent) {
    // TODO fix this crazy stuff here : convert latlon to scene, scenePos to meters, then back to scenePos...
    // sol1: get latlon of mouse, distance in meters with center
    // sol2: pass directly distance in scenePos to child circle, then update th radius somehow.
    QPointF center = scenePoint(cir->position(), zoomLevel(map->zoom()), map->tileSize());
    Point2DTile center_tile = Point2DTile(cir->position(), zoomLevel(map->zoom()));
    QPointF dp = mouseEvent->scenePos()- center;
    double d_tile = sqrt(dp.x()*dp.x() + dp.y()*dp.y()) / map->tileSize();
    double d_meters = distTile2Meters(center_tile.y(), d_tile, zoomLevel(map->zoom()));
    //qDebug() << "Circle state machine: moved: " << d << "   " << d2;
    cir->setRadius(d_meters);
}
