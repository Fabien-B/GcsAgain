#include "aircraft.h"
#include "pprz_dispatcher.h"
#include "dispatcher_ui.h"
#include <QDebug>
#include "AircraftManager.h"

Aircraft::Aircraft(ConfigData* config, QObject* parent): QObject(parent),
    ac_id(config->getId()), color(config->getColor()), _name(config->getName()), config(config), position(Point2DLatLon(0,0)), real(true)
{
    config->setParent(this);
    QSettings app_settings(qApp->property("SETTINGS_PATH").toString(), QSettings::IniFormat);
    flight_plan = new FlightPlan(ac_id, config->getFlightPlan(), this);
    setting_menu = new SettingMenu(config->getSettings(), this);
    airframe = new Airframe(config->getAirframe(), this);
    icon = app_settings.value("path/aircraft_icon").toString() + "/" + QString(airframe->getIconName()) + ".svg";
    status = new AircraftStatus(ac_id, this);
}

Aircraft::Aircraft(QString ac_id, QString flightplan, QObject* parent): QObject(parent),
    ac_id(ac_id), _name(ac_id), config(nullptr), position(Point2DLatLon(0,0)), real(false)
{
    static int last_color = (int) Qt::red;
    color = QColor((Qt::GlobalColor)last_color++);
    QFile f(flightplan);
    if(!f.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Error while loading flightplan file");
    }
    QDomDocument fp;
    fp.setContent(&f);
    f.close();
    flight_plan = new FlightPlan(ac_id, fp, this);
    setting_menu = nullptr;
    airframe = nullptr;
    status = nullptr;
}

void Aircraft::setSetting(Setting* setting, float value) {
    auto coef = setting->getAltUnitCoef();
    if(value < setting->getMin() / coef || value > setting->getMax() / coef) {
        qDebug() << "Warning: send setting value " << value << ", out of bounds for setting " << setting->getName();
    }

    pprzlink::Message dlSetting(PprzDispatcher::get()->getDict()->getDefinition("DL_SETTING"));
    dlSetting.addField("ac_id", ac_id);
    dlSetting.addField("index", setting->getNo());
    dlSetting.addField("value", value);
    PprzDispatcher::get()->sendMessage(dlSetting);
}

void Aircraft::setSetting(uint8_t setting_no, float value) {
    for(auto set: setting_menu->getAllSettings()) {
        if(set->getNo() == setting_no) {
            setSetting(set, value);
            break;
        }
    }
}
