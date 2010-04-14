/* * This file is part of m-im-framework *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "minputcontextdbusconnection.h"
#include "minputcontextdbusconnection_p.h"

#include <QObject>
#include <QDebug>
#include <QRegion>
#include <QKeyEvent>
#include <QDBusAbstractAdaptor>
#include <QDBusArgument>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusReply>
#include <QMap>
#include <QString>
#include <QVariant>

#include "minputmethodbase.h"

namespace
{
    // FIXME: using dui prefix
    const char * const DBusServiceName = "org.maemo.duiinputmethodserver1";
    const char * const DBusPath = "/org/maemo/duiinputmethodserver1";
    // FIXME: unused, directly in header
    const char * const DBusInterface = "org.maemo.duiinputmethodserver1";

    const char * const DBusClientInterface = "org.maemo.duiinputcontext1";

    // attribute names for updateWidgetInformation() map
    const char * const ContentTypeAttribute = "contentType";
    const char * const CorrectionAttribute = "correctionEnabled";
    const char * const PredictionAttribute = "predictionEnabled";
    const char * const AutoCapitalizationAttribute = "autocapitalizationEnabled";
    const char * const SurroundingTextAttribute = "surroundingText";
    const char * const CursorPositionAttribute = "cursorPosition";
    const char * const HasSelectionAttribute = "hasSelection";
    const char * const InputMethodModeAttribute = "inputMethodMode";
    const char * const VisualizationAttribute = "visualizationPriority";
    const char * const ToolbarAttribute = "toolbar";
}



MInputContextDBusAdaptor::MInputContextDBusAdaptor(MInputContextDBusConnection *host)
    : QDBusAbstractAdaptor(host),
      host(host)
{
    // nothing
}


MInputContextDBusAdaptor::~MInputContextDBusAdaptor()
{
    // nothing
}


void MInputContextDBusAdaptor::setContextObject(QString callbackObject)
{
    host->setContextObject(callbackObject);
}


void MInputContextDBusAdaptor::activateContext()
{
    host->activateContext();
}


void MInputContextDBusAdaptor::showInputMethod()
{
    host->showInputMethod();
}


void MInputContextDBusAdaptor::hideInputMethod()
{
    host->hideInputMethod();
}


void MInputContextDBusAdaptor::mouseClickedOnPreedit(const QPoint &pos, const QRect &preeditRect)
{
    host->mouseClickedOnPreedit(pos, preeditRect);
}


void MInputContextDBusAdaptor::setPreedit(const QString &text)
{
    host->setPreedit(text);
}

void MInputContextDBusAdaptor::updateWidgetInformation(const QMap<QString, QVariant> &stateInfo)
{
    host->updateWidgetInformation(stateInfo);
}


void MInputContextDBusAdaptor::reset()
{
    host->reset();
}

void MInputContextDBusAdaptor::appOrientationChanged(int angle)
{
    host->appOrientationChanged(angle);
}

void MInputContextDBusAdaptor::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
{
    host->setCopyPasteState(copyAvailable, pasteAvailable);
}

void MInputContextDBusAdaptor::processKeyEvent(int keyType, int keyCode, int modifiers,
                                               const QString &text, bool autoRepeat,
                                               int count, int nativeScanCode)
{
    host->processKeyEvent(static_cast<QEvent::Type>(keyType), static_cast<Qt::Key>(keyCode),
                          static_cast<Qt::KeyboardModifiers>(modifiers), text, autoRepeat, count, nativeScanCode);
}


////////////////
// private class


MInputContextDBusConnectionPrivate::MInputContextDBusConnectionPrivate()
    : activeContext(0),
      valid(true),
      globalCorrectionEnabled(false),
      redirectionEnabled(false)
{
    // nothing
}


MInputContextDBusConnectionPrivate::~MInputContextDBusConnectionPrivate()
{
    QMapIterator<QString, QDBusInterface *> it(clients);

    while (it.hasNext()) {
        it.next();
        delete it.value();
    }
}



///////////////
// actual class


MInputContextDBusConnection::MInputContextDBusConnection()
    : d(new MInputContextDBusConnectionPrivate)
{
    new MInputContextDBusAdaptor(this);
    bool success = QDBusConnection::sessionBus().registerObject(DBusPath, this);

    if (!success) {
        d->valid = false;
        qDebug() << "MInputContextDBusConnection failed to register D-Bus object";
    }

    if (!QDBusConnection::sessionBus().registerService(DBusServiceName)) {
        d->valid = false;
        qDebug() << "MInputContextDBusConnection failed to register D-Bus service";
        qDebug() << QDBusConnection::sessionBus().lastError().message();
    }

    qDBusRegisterMetaType<QRect>();
    qDBusRegisterMetaType<QMap<QString, QVariant> >();
}


MInputContextDBusConnection::~MInputContextDBusConnection()
{
    delete d;
}


bool MInputContextDBusConnection::isValid()
{
    return d->valid;
}


void MInputContextDBusConnection::sendPreeditString(const QString &string,
        PreeditFace preeditFace)
{
    if (d->activeContext) {
        d->activeContext->call("updatePreedit", string, preeditFace);
    }
}


void MInputContextDBusConnection::sendCommitString(const QString &string)
{
    if (d->activeContext) {
        d->activeContext->call("commitString", string);
    }
}


void MInputContextDBusConnection::sendKeyEvent(const QKeyEvent &keyEvent)
{
    if (d->activeContext) {
        int type = static_cast<int>(keyEvent.type());
        int key = static_cast<int>(keyEvent.key());
        int modifiers = static_cast<int>(keyEvent.modifiers());

        d->activeContext->call("keyEvent", type, key, modifiers, keyEvent.text(),
                               keyEvent.isAutoRepeat(), keyEvent.count());
    }
}


void MInputContextDBusConnection::notifyImInitiatedHiding()
{
    if (d->activeContext) {
        d->activeContext->call("imInitiatedHide");
    }
}


int MInputContextDBusConnection::contentType(bool &valid)
{
    QVariant contentTypeVariant = d->widgetState[ContentTypeAttribute];
    return contentTypeVariant.toInt(&valid);
}


bool MInputContextDBusConnection::correctionEnabled(bool &valid)
{
    QVariant correctionVariant = d->widgetState[CorrectionAttribute];
    valid = correctionVariant.isValid();
    return correctionVariant.toBool();
}


bool MInputContextDBusConnection::predictionEnabled(bool &valid)
{
    QVariant predictionVariant = d->widgetState[PredictionAttribute];
    valid = predictionVariant.isValid();
    return predictionVariant.toBool();
}


bool MInputContextDBusConnection::autoCapitalizationEnabled(bool &valid)
{
    QVariant capitalizationVariant = d->widgetState[AutoCapitalizationAttribute];
    valid = capitalizationVariant.isValid();
    return capitalizationVariant.toBool();
}


void MInputContextDBusConnection::setGlobalCorrectionEnabled(bool enabled)
{
    if ((enabled != d->globalCorrectionEnabled) && d->activeContext) {
        d->activeContext->call("setGlobalCorrectionEnabled", enabled);
    }

    d->globalCorrectionEnabled = enabled;
}


bool MInputContextDBusConnection::surroundingText(QString &text, int &cursorPosition)
{
    QVariant textVariant = d->widgetState[SurroundingTextAttribute];
    QVariant posVariant = d->widgetState[CursorPositionAttribute];

    if (textVariant.isValid() && posVariant.isValid()) {
        text = textVariant.toString();
        cursorPosition = posVariant.toInt();
        return true;
    }

    return false;
}


bool MInputContextDBusConnection::hasSelection(bool &valid)
{
    QVariant selectionVariant = d->widgetState[HasSelectionAttribute];
    valid = selectionVariant.isValid();
    return selectionVariant.toBool();
}


int MInputContextDBusConnection::inputMethodMode(bool &valid)
{
    QVariant modeVariant = d->widgetState[InputMethodModeAttribute];
    return modeVariant.toInt(&valid);
}


QRect MInputContextDBusConnection::preeditRectangle(bool &valid)
{
    QRect rect;
    valid = false;
    if (d->activeContext) {
        QDBusMessage reply = d->activeContext->call("preeditRectangle");

        if (reply.type() == QDBusMessage::ReplyMessage) {
            QList<QVariant> arguments = reply.arguments();
            rect = qdbus_cast<QRect>(arguments.at(0).value<QDBusArgument>());
            valid = arguments.at(1).toBool();
        }
    }

    return rect;
}

void MInputContextDBusConnection::setRedirectKeys(bool enabled)
{
    if ((d->redirectionEnabled != enabled) && d->activeContext) {
        d->activeContext->call("setRedirectKeys", enabled);
    }

    d->redirectionEnabled = enabled;
}


void MInputContextDBusConnection::copy()
{
    if (d->activeContext) {
        d->activeContext->call("copy");
    }
}


void MInputContextDBusConnection::paste()
{
    if (d->activeContext) {
        d->activeContext->call("paste");
    }
}



//void MInputContextDBusConnection::setContextObject(QDBusObjectPath callbackObject)
void MInputContextDBusConnection::setContextObject(QString callbackObject)
{
    qDebug() << "in" << __PRETTY_FUNCTION__;

    QDBusConnection conn = connection();
    const QDBusMessage &msg = message();

    //qDebug() << "base service:" << conn.baseService();
    //qDebug() << "message:" << msg.member() << msg.service() << msg.path() << msg.interface();
    //qDebug() << "callbackobject:" << callbackObject;

    // delete existing dbus interface object in case this method gets called multiple times
    if (d->clients.contains(msg.service())) {
        delete d->clients[msg.service()];
    }

    // creates interface object for the caller
    // (using no explicit session bus, no parent (FIXME?))
    QDBusInterface *clientConnection = new QDBusInterface(msg.service(), callbackObject,
            DBusClientInterface);
    if (clientConnection->isValid() == false) {
        qDebug() << "ERROR: DBus connection to client input context cannot be made";
        qDebug() << "msg:" << QDBusConnection::sessionBus().lastError().message();
    }

    d->clients[msg.service()] = clientConnection;
}


void MInputContextDBusConnection::activateContext()
{
    qDebug() << "in" << __PRETTY_FUNCTION__;

    const QDBusMessage &msg = message();
    QDBusInterface *previousActive = d->activeContext;

    // set active context to interface corresponding caller
    if (d->clients.contains(msg.service())) {
        d->activeContext = d->clients[msg.service()];

        // update the state of the new active context
        d->activeContext->call("setGlobalCorrectionEnabled", d->globalCorrectionEnabled);
        d->activeContext->call("setRedirectKeys", d->redirectionEnabled);

    } else {
        qDebug() << __PRETTY_FUNCTION__ << "unable to activate context";
        d->activeContext = 0;
    }

    if ((previousActive != 0) && (previousActive != d->activeContext)) {
        previousActive->call("activationLostEvent");
    }

    // notify plugins
    foreach (MInputMethodBase *target, targets()) {
        target->clientChanged();
    }
}


void MInputContextDBusConnection::showInputMethod()
{
    foreach (MInputMethodBase *target, targets()) {
        target->show();
    }
}


void MInputContextDBusConnection::hideInputMethod()
{
    foreach (MInputMethodBase *target, targets()) {
        target->hide();
    }
}


void MInputContextDBusConnection::mouseClickedOnPreedit(const QPoint &pos,
                                                        const QRect &preeditRect)
{
    foreach (MInputMethodBase *target, targets()) {
        target->mouseClickedOnPreedit(pos, preeditRect);
    }
}


void MInputContextDBusConnection::setPreedit(const QString &text)
{
    foreach (MInputMethodBase *target, targets()) {
        target->setPreedit(text);
    }
}


void MInputContextDBusConnection::reset()
{
    foreach (MInputMethodBase *target, targets()) {
        target->reset();
    }
}


void
MInputContextDBusConnection::updateWidgetInformation(const QMap<QString, QVariant> &stateInfo)
{
    // check visualization change
    bool oldVisualization = false;
    bool newVisualization = false;

    QVariant variant = d->widgetState[VisualizationAttribute];

    if (variant.isValid()) {
        oldVisualization = variant.toBool();
    }

    variant = stateInfo[VisualizationAttribute];
    if (variant.isValid()) {
        newVisualization = variant.toBool();
    }

    // toolbar change
    QString oldToolbar;
    QString newToolbar;
    variant = d->widgetState[ToolbarAttribute];

    if (variant.isValid()) {
        oldToolbar = variant.toString();
    }

    variant = stateInfo[ToolbarAttribute];
    if (variant.isValid()) {
        newToolbar = variant.toString();
    }

    // update state
    d->widgetState = stateInfo;

    // call notification methods if needed
    if (oldVisualization != newVisualization) {
        foreach (MInputMethodBase *target, targets()) {
            target->visualizationPriorityChanged(newVisualization);
        }
    }

    if (oldToolbar != newToolbar) {
        foreach (MInputMethodBase *target, targets()) {
            target->setToolbar(newToolbar);
        }
    }

    // general notification last
    foreach (MInputMethodBase *target, targets()) {
        target->update();
    }
}


void MInputContextDBusConnection::appOrientationChanged(int angle)
{
    foreach (MInputMethodBase *target, targets()) {
        target->appOrientationChanged(angle);
    }
}


void MInputContextDBusConnection::updateInputMethodArea(const QRegion &region)
{
    if (d->activeContext) {
        QList<QVariant> data;
        data.append(region.boundingRect());
        d->activeContext->call("updateInputMethodArea", data);
    }
}


void MInputContextDBusConnection::setCopyPasteState(bool copyAvailable, bool pasteAvailable)
{
    foreach (MInputMethodBase *target, targets()) {
        target->setCopyPasteState(copyAvailable, pasteAvailable);
    }
}


void MInputContextDBusConnection::processKeyEvent(QEvent::Type keyType, Qt::Key keyCode,
                                                  Qt::KeyboardModifiers modifiers,
                                                  const QString &text, bool autoRepeat, int count,
                                                  int nativeScanCode)
{
    foreach (MInputMethodBase *target, targets()) {
        target->processKeyEvent(keyType, keyCode, modifiers, text, autoRepeat, count,
                                nativeScanCode);
    }
}