#include "SSDPDiscovery.h"

// DLNA renderers discovery class
SSDPdiscovery::SSDPdiscovery(QObject *parent) : QObject(parent)
{
    // DLNA discovery adress
    QHostAddress groupAddress = QHostAddress("239.255.255.250");

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::AnyIPv4, 1901, QUdpSocket::ShareAddress);
    udpSocket->joinMulticastGroup(groupAddress);

    connect(udpSocket, SIGNAL(readyRead()),
            this, SLOT(processPendingDatagrams()));

    QByteArray datagram;
    // Multicast message, used for discovery of DLNA devices
    datagram.append( "M-SEARCH * HTTP/1.1\r\n" );
    datagram.append( "HOST: 239.255.255.250:1900\r\n" );
    datagram.append( "MX: 5\nMan: \"ssdp:discover\"\r\n" );
    datagram.append( "ST: urn:schemas-upnp-org:device:MediaRenderer:1\r\n\r\n" );

    udpSocket->writeDatagram(datagram.data(), datagram.size(), groupAddress, 1900);
}

// Process received datagrams, and get IP of DLNA DMR
void SSDPdiscovery::processPendingDatagrams()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        QString data = datagram.data();
        QStringList list = data.split("\n");

        for(int i = 0; i<list.size(); ++i)
        {
            // Get url of ip of renderer, send request to get more data
            if(list[i].startsWith("Location:"))
            {
                nmgr = new QNetworkAccessManager(this);
                reply = nmgr->get(QNetworkRequest(list[i].mid(10).simplified()));
                connect(reply, SIGNAL(finished()), this, SLOT(processData()));
                break;
            }
        }
    }
}

// This function process request data, into DLNA Server
void SSDPdiscovery::processData()
{
    QXmlStreamReader xml(reply->readAll());
    reply->close();

    // Construct renderer object
    DLNARenderer *renderer = new DLNARenderer(reply->url(), this);

    // Parse return url
    while(!xml.hasError() && !xml.atEnd())
    {
        xml.readNextStartElement();
        if(xml.name()=="serviceId" && xml.readElementText()=="urn:upnp-org:serviceId:AVTransport")
        {
            while(!(xml.name()=="controlURL") && !xml.atEnd())
                xml.readNextStartElement();
            if(xml.name()=="controlURL")
                renderer->setControlUrl(xml.readElementText());
        }
        else if(xml.name()=="friendlyName")
            renderer->setName(xml.readElementText());
    }
    emit foundRenderer(renderer);
}