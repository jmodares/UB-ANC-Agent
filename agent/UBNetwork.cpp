#include "UBNetwork.h"
#include "UBPacket.h"

#include "UBConfig.h"

#include <QHostAddress>

UBNetwork::UBNetwork(QTcpSocket *parent) : QTcpSocket(parent),
    m_id(0)
{
    // readyRead is emitted when data is available to read from the TCP socket
    connect(this, SIGNAL(readyRead()), this, SLOT(dataReadyEvent()));
}

void UBNetwork::sendData(quint8 desID, QByteArray data) {
    UBPacket packet;
    packet.setSrcID(m_id);
    packet.setDesID(desID);
    packet.setPayload(data);

    write(packet.packetize().append(PACKET_END));
}

void UBNetwork::dataReadyEvent() {
    m_data += readAll(); // Read bytestream from TCP socket

    while (true) {
        // PACKET_END in bytestream indicates that an entire packet as been read
        int bytes = m_data.indexOf(PACKET_END);
        if (bytes == -1) {
            break;
        }

        UBPacket packet;
        packet.depacketize(m_data.left(bytes));
        m_data.remove(0, bytes + qstrlen(PACKET_END));

        if (packet.getDesID() == m_id || packet.getDesID() == BROADCAST_ID) {
            emit dataReady(packet.getSrcID(), packet.getPayload());

            qInfo() << "Packet Received | From " << packet.getSrcID() << " to " << packet.getDesID() << " | Size: " << packet.getPayload().size();
        }
    }
}
