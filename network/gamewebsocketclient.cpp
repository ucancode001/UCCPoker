#include "gamewebsocketclient.h"

#include <QAbstractSocket>
#include <QJsonDocument>

GameWebSocketClient::GameWebSocketClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_socket, &QWebSocket::connected, this, [this]() {
        m_reconnectAttempts = 0;
        flushPendingMessages();
        emit connected();
    });
    connect(&m_socket, &QWebSocket::disconnected, this, &GameWebSocketClient::onDisconnected);
    connect(&m_socket, &QWebSocket::textMessageReceived, this, &GameWebSocketClient::onTextMessageReceived);
    connect(&m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            this, [this](QAbstractSocket::SocketError) {
                emit connectionError(m_socket.errorString());
            });

    m_reconnectTimer.setSingleShot(true);
    connect(&m_reconnectTimer, &QTimer::timeout, this, &GameWebSocketClient::tryReconnect);
}

void GameWebSocketClient::setAutoReconnect(bool enabled)
{
    m_autoReconnect = enabled;
}

void GameWebSocketClient::setMaxReconnectAttempts(int attempts)
{
    m_maxReconnectAttempts = qMax(1, attempts);
}

void GameWebSocketClient::connectToServer(const QString &url)
{
    m_manualDisconnect = false;
    m_url = url;
    m_reconnectAttempts = 0;
    if (m_socket.state() != QAbstractSocket::UnconnectedState)
        m_socket.close();
    m_socket.open(QUrl(url));
}

void GameWebSocketClient::disconnectFromServer()
{
    m_manualDisconnect = true;
    m_reconnectTimer.stop();
    m_pendingMessages.clear();
    m_socket.close();
}

bool GameWebSocketClient::isConnected() const
{
    return m_socket.state() == QAbstractSocket::ConnectedState;
}

void GameWebSocketClient::enqueueMessage(int code, const QJsonObject &data)
{
    PendingMessage message;
    message.code = code;
    message.data = data;
    m_pendingMessages.append(message);
}

void GameWebSocketClient::flushPendingMessages()
{
    if (!isConnected())
        return;

    const QList<PendingMessage> pending = m_pendingMessages;
    m_pendingMessages.clear();
    for (const PendingMessage &message : pending)
        sendMessage(message.code, message.data, false);
}

bool GameWebSocketClient::sendMessage(int code, const QJsonObject &data, bool queueIfOffline)
{
    if (!isConnected()) {
        if (queueIfOffline && m_autoReconnect && !m_manualDisconnect) {
            enqueueMessage(code, data);
            return true;
        }
        emit sendFailed(code, QStringLiteral("未连接到服务器"));
        return false;
    }

    QJsonObject envelope;
    envelope.insert(QStringLiteral("code"), code);
    envelope.insert(QStringLiteral("data"), data);
    m_socket.sendTextMessage(QString::fromUtf8(QJsonDocument(envelope).toJson(QJsonDocument::Compact)));
    return true;
}

void GameWebSocketClient::onTextMessageReceived(const QString &message)
{
    if (message.trimmed().isEmpty())
        return;

    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        emit connectionError(QStringLiteral("消息解析失败：%1").arg(error.errorString()));
        return;
    }

    const QJsonObject root = doc.object();
    emit roomMessageReceived(root.value(QStringLiteral("code")).toInt(),
                             root.value(QStringLiteral("data")).toObject());
}

void GameWebSocketClient::onDisconnected()
{
    emit disconnected();
    if (m_manualDisconnect || !m_autoReconnect || m_url.isEmpty())
        return;
    if (m_reconnectAttempts >= m_maxReconnectAttempts)
        return;

    const int delay = qMin(1000 * (1 << m_reconnectAttempts), 8000);
    m_reconnectTimer.start(delay);
}

void GameWebSocketClient::tryReconnect()
{
    if (m_manualDisconnect || m_url.isEmpty())
        return;

    ++m_reconnectAttempts;
    emit reconnecting(m_reconnectAttempts, m_maxReconnectAttempts);
    m_socket.open(QUrl(m_url));
}
