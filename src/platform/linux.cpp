#include "util.h"
#include "settings.h"

#include <QRegularExpression>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QDir>
#include <QUrl>
#include <QGuiApplication>

#if defined(BAKA_HAVE_X11) && QT_CONFIG(xcb)
#include <X11/Xlib.h>
#define BAKA_X11 1
#endif

namespace Util {

#ifdef BAKA_X11
static Display *X11Display()
{
    if(auto *x11 = qApp->nativeInterface<QNativeInterface::QX11Application>())
        return x11->display();
    return nullptr;
}
#endif

bool DimLightsSupported()
{
#ifdef BAKA_X11
    Display *display = X11Display();
    if(!display)
        return false;
    QString tmp = "_NET_WM_CM_S"+QString::number(DefaultScreen(display));
    Atom a = XInternAtom(display, tmp.toUtf8().constData(), false);
    if(a && XGetSelectionOwner(display, a)) // is a compositing manager running?
        return true;
#endif
    return false;
}

void SetAlwaysOnTop(WId wid, bool ontop)
{
#ifdef BAKA_X11
    Display *display = X11Display();
    if(!display)
        return; // not supported on wayland
    XEvent event;
    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.display = display;
    event.xclient.window  = wid;
    event.xclient.message_type = XInternAtom (display, "_NET_WM_STATE", False);
    event.xclient.format = 32;

    event.xclient.data.l[0] = ontop;
    event.xclient.data.l[1] = XInternAtom (display, "_NET_WM_STATE_ABOVE", False);
    event.xclient.data.l[2] = 0; //unused.
    event.xclient.data.l[3] = 0;
    event.xclient.data.l[4] = 0;

    XSendEvent(display, DefaultRootWindow(display), False,
                           SubstructureRedirectMask|SubstructureNotifyMask, &event);
    XFlush(display);
#else
    Q_UNUSED(wid);
    Q_UNUSED(ontop);
#endif
}

QString SettingsLocation()
{
    // saves to  ~/.config/${SETTINGS_FILE}.ini
    return QString("%0/%1.ini").arg(
            QStandardPaths::writableLocation(QStandardPaths::ConfigLocation),
            SETTINGS_FILE);
}

bool IsValidFile(QString path)
{
    static const QRegularExpression rx("^\\.{1,2}|/", QRegularExpression::CaseInsensitiveOption); // relative path, network location, drive
    return rx.match(path).hasMatch();
}

bool IsValidLocation(QString loc)
{
    static const QRegularExpression rx("^([a-z]{2,}://|\\.{1,2}|/)", QRegularExpression::CaseInsensitiveOption); // url, relative path, absolute path
    return rx.match(loc).hasMatch();
}

void ShowInFolder(QString path, QString)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

QString MonospaceFont()
{
    return "Monospace";
}

}
