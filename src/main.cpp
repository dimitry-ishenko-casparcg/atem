////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2021 Dimitry Ishenko
// Contact: dimitry (dot) ishenko (at) (gee) mail (dot) com
//
// Distributed under the GNU GPL license. See the LICENSE.md file for details.

////////////////////////////////////////////////////////////////////////////////
#include <qatemconnection.h>
#include <qatemmixeffect.h>

#include <QCoreApplication>
#include <QHostInfo>
#include <QObject>
#include <QString>
#include <QTimer>

#include <iostream>
#include <stdexcept>
#include <string>

enum command { none, fade, cut, pvw, pgm };

////////////////////////////////////////////////////////////////////////////////
void show_usage(const QString& name)
{
    std::cout << "\nUsage: " << name.toStdString() << " <addr> <m/e> <cmd> [<args>...]\n" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void throw_invalid(const QString& message)
{
    throw std::invalid_argument{ message.toStdString() + "." };
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
try
{
    QCoreApplication app{ argc, argv };

    if(argc < 2)
    {
        show_usage(app.applicationName());
        throw_invalid("Missing ATEM address");
    }

    auto info = QHostInfo::fromName(argv[1]);
    if(info.error() != QHostInfo::NoError) throw_invalid(info.errorString());

    auto addresses = info.addresses();
    if(addresses.empty()) throw_invalid("ATEM address not found");

    if(argc < 3)
    {
        show_usage(app.applicationName());
        throw_invalid("Missing ATEM M/E #");
    }

    bool ok = true;
    auto me_n = QString{ argv[2] }.toUInt(&ok);
    if(!ok) throw_invalid("Invalid ATEM M/E #");

    if(argc < 4)
    {
        show_usage(app.applicationName());
        throw_invalid("Missing ATEM command");
    }

    command cmd = none;
    quint16 in_n;

    QString cmd_s{ argv[3] };
         if(cmd_s == "fade") cmd = fade;
    else if(cmd_s == "cut" ) cmd = cut;
    else if(cmd_s == "pvw" )
    {
        cmd = pvw;

        if(argc < 5)
        {
            show_usage(app.applicationName());
            throw_invalid("Missing input #");
        }

        bool ok = true;
        in_n = QString{ argv[4] }.toUInt(&ok);
        if(!ok) throw_invalid("Invalid preview #");
    }
    else if(cmd_s == "pgm" )
    {
        cmd = pgm;

        if(argc < 5)
        {
            show_usage(app.applicationName());
            throw_invalid("Missing input #");
        }

        bool ok = true;
        in_n = QString{ argv[4] }.toUInt(&ok);
        if(!ok) throw_invalid("Invalid program #");
    }
    else throw_invalid("Invalid ATEM command");

    QAtemConnection atem;
    QTimer::singleShot(0, [&](){ atem.connectToSwitcher(addresses[0]); });

    QObject::connect(&atem, &QAtemConnection::topologyChanged, [&]()
    {
        auto me = atem.mixEffect(me_n);
        if(!me) throw_invalid("ATEM M/E not found");

        switch(cmd)
        {
        case fade: me->autoTransition(); break;
        case cut:  me->cut(); break;
        case pvw:  me->changePreviewInput(in_n); break;
        case pgm:  me->changeProgramInput(in_n); break;
        default: break;
        }

        app.exit(0);
    });
    QObject::connect(&atem, &QAtemConnection::disconnected, &app, &QCoreApplication::quit);
    QObject::connect(&atem, &QAtemConnection::socketError, &throw_invalid);

    return app.exec();
}
catch(std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}
