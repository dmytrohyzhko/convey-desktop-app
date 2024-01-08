#ifndef PACKETTYPE_H
#define PACKETTYPE_H

#include <QString>

class PacketType
{
public:
    QString type;
public:
    PacketType();
};

class PacketWindows : public PacketType {
public:
    PacketWindows() {

    }
};

class PacketWindowsAck : public PacketType {
public:
    PacketWindowsAck() {

    }

};

#endif // PACKETTYPE_H
