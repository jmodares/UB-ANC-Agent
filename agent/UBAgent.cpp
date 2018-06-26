#include "UBAgent.h"
#include "UBNetwork.h"

#include "UBConfig.h"

#include <QTimer>
#include <QCommandLineParser>

#include "Vehicle.h"
#include "TCPLink.h"
#include "QGCApplication.h"


UBAgent::UBAgent(QObject *parent) : QObject(parent),
    m_mav(nullptr)
{
    m_net = new UBNetwork;
    connect(m_net, SIGNAL(dataReady(quint8, QByteArray)), this, SLOT(dataReadyEvent(quint8, QByteArray)));

    m_timer = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(missionTracker()));

    startAgent();
}

void UBAgent::startAgent() {
    QCommandLineParser parser;
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    parser.addOptions({
        {{"I", "instance"}, "Set instance (ID) of the agent", "id"},
    });

//    parser.process(*QCoreApplication::instance());
    parser.parse(QCoreApplication::arguments());

    quint8 id = parser.value("I").toUInt();
    LinkConfiguration* link = nullptr;
    if (id) {
        quint32 port = 10 * id + STL_PORT + 3;
        TCPConfiguration* tcp = new TCPConfiguration(tr("TCP Port %1").arg(port));
        tcp->setAddress(QHostAddress::LocalHost);
        tcp->setPort(port);

        link = tcp;
    } else {
        SerialConfiguration* serial = new SerialConfiguration("Serial Port");
        serial->setBaud(BAUD_RATE);
        serial->setPortName(SERIAL_PORT);

        link = serial;
    }

    link->setDynamic();
    link->setAutoConnect();

    LinkManager* linkManager = qgcApp()->toolbox()->linkManager();
    linkManager->addConfiguration(link);
    linkManager->linkConfigurationsChanged();

    connect(qgcApp()->toolbox()->multiVehicleManager(), SIGNAL(vehicleAdded(Vehicle*)), this, SLOT(vehicleAddedEvent(Vehicle*)));
    connect(qgcApp()->toolbox()->multiVehicleManager(), SIGNAL(vehicleRemoved(Vehicle*)), this, SLOT(vehicleRemovedEvent(Vehicle*)));

    m_net->connectToHost(QHostAddress::LocalHost, 10 * id + NET_PORT);
    m_timer->start(1000.0 * MISSION_TRACK_PERIOD);    // m_timer in milliseconds
}

void UBAgent::setMAV(Vehicle* mav) {
    // If m_mav is already initialized, then disconnect it first
    if (m_mav) {
        disconnect(m_mav, SIGNAL(armedChanged(bool)), this, SLOT(armedChangedEvent(bool)));
        disconnect(m_mav, SIGNAL(flightModeChanged(QString)), this, SLOT(flightModeChangedEvent(QString)));
    }

    m_mav = mav;

    // Connect new mav
    if (m_mav) {
        connect(m_mav, SIGNAL(armedChanged(bool)), this, SLOT(armedChangedEvent(bool)));
        connect(m_mav, SIGNAL(flightModeChanged(QString)), this, SLOT(flightModeChangedEvent(QString)));
    }
}

void UBAgent::vehicleAddedEvent(Vehicle* mav) {
    if (!mav || m_mav == mav) {
        return;
    }

    setMAV(mav);
    m_net->setID(mav->id());

    qInfo() << "New MAV connected with ID: " << m_mav->id();
}

void UBAgent::vehicleRemovedEvent(Vehicle* mav) {
    if (!mav || m_mav != mav) {
        return;
    }

    setMAV(nullptr);
    m_net->setID(0);

    qInfo() << "MAV disconnected with ID: " << mav->id();
}

// Handle arm/disarm event
void UBAgent::armedChangedEvent(bool armed) {
    if (!armed) {
        m_mission_state = STATE_IDLE;
        return;
    }

    if (m_mav->altitudeRelative()->rawValue().toDouble() > POINT_ZONE) {
        qWarning() << "The mission cannot start if the drone is airborne!";
        return;
    }

    /********************** IMPORTANT SAFETY INFORMATION **********************
       We require that the drone is set to Guided mode before it is armed.
       Since we allow the drone to be armed by another drone (over the network),
       this extra step ensures that the operator is ready
       before the drone takes off.
    **************************************************************************/
//    m_mav->setGuidedMode(true);
    if (!m_mav->guidedMode()) {
        qWarning() << "The mission cannot start unless the drone is in Guided mode!";
        return;
    }

    m_mission_data.reset();
    m_mission_state = STATE_TAKEOFF;
    qInfo() << "Mission starts...";

//    m_mav->guidedModeTakeoff();
    m_mav->sendMavCommand(m_mav->defaultComponentId(),
                            MAV_CMD_NAV_TAKEOFF,
                            true, // show error
                            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                            TAKEOFF_ALT);
}

void UBAgent::flightModeChangedEvent(QString mode) {
    qInfo() << mode;
}

// Process/handle received packets
void UBAgent::dataReadyEvent(quint8 srcID, QByteArray data) {
    Q_UNUSED(data)
    if(srcID == m_mav->id() - 1 && !m_mav->armed()) {
        // setArmed() is not always successful
        // If it is successful, UBAgent::armedChangedEvent() will be called
        m_mav->setArmed(true);
    }
}

// Mission logic
// This example mission is implemented as a state machine
void UBAgent::missionTracker() {
    switch (m_mission_state) {
    case STATE_IDLE:
        stageIdle();
        break;
    case STATE_TAKEOFF:
        stageTakeoff();
        break;
    case STATE_MISSION:
        stageMission();
        break;
    case STATE_LAND:
        stageLand();
        break;
    default:
        break;
    }
}

void UBAgent::stageIdle() {
    // Do nothing
}

void UBAgent::stageTakeoff() {
    // Check if target altitude has been reached (within tolerance)
    if (m_mav->altitudeRelative()->rawValue().toDouble() > TAKEOFF_ALT - POINT_ZONE) {
        m_mission_data.stage = 0;
        m_mission_state = STATE_MISSION;
    }
}

void UBAgent::stageLand() {
    // Check if near ground (within tolerance)
    if (m_mav->altitudeRelative()->rawValue().toDouble() < POINT_ZONE) {
        m_mission_state = STATE_IDLE;
        qInfo() << "Mission ends";
    }
}

void UBAgent::stageMission() {
    static QGeoCoordinate dest;

    // 0th stage of the mission (after taking off, before landing)
    if (m_mission_data.stage == 0) {
        m_mission_data.stage++;

        // Set destination 10 meters to the East
        dest = m_mav->coordinate().atDistanceAndAzimuth(10, 90); // 0 -> North; 90 (M_PI / 2) -> East
        m_mav->guidedModeGotoLocation(dest);

        return;
    }

    // 1st stage of the mission
    if (m_mission_data.stage == 1) {
        // Check if target destination has been reached (within tolerance)
        if (m_mav->coordinate().distanceTo(dest) < POINT_ZONE) {
            m_mission_data.stage++;
        }

        return;
    }

    // 2nd stage of the mission
    // Wait 20 seconds before switching to Land mode
    if (m_mission_data.tick < (20.0 / MISSION_TRACK_PERIOD)) {
        m_mission_data.tick++;

        // Send packet to next mav in the chain
        m_net->sendData(m_mav->id() + 1, QByteArray(1, MAV_CMD_NAV_TAKEOFF));
    } else {
        m_mav->guidedModeLand();
        m_mission_state = STATE_LAND;
    }
}
