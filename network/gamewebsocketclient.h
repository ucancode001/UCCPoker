#ifndef GAMEWEBSOCKETCLIENT_H
#define GAMEWEBSOCKETCLIENT_H

#include <QJsonObject>
#include <QObject>
#include <QTimer>
#include <QWebSocket>

class GameWebSocketClient : public QObject
{
    Q_OBJECT

public:
    struct PendingMessage {
        int code = 0;
        QJsonObject data;
    };

    explicit GameWebSocketClient(QObject *parent = nullptr);

    void setAutoReconnect(bool enabled);
    void setMaxReconnectAttempts(int attempts);

    void connectToServer(const QString &url);
    void disconnectFromServer();
    bool sendMessage(int code, const QJsonObject &data, bool queueIfOffline = true);
    bool isConnected() const;
    int reconnectAttempts() const { return m_reconnectAttempts; }
    int pendingMessageCount() const { return m_pendingMessages.size(); }

signals:
    void connected();
    void disconnected();
    void connectionError(const QString &message);
    void reconnecting(int attempt, int maxAttempts);
    void sendFailed(int code, const QString &reason);
    void roomMessageReceived(int code, const QJsonObject &data);

private slots:
    void onTextMessageReceived(const QString &message);
    void onDisconnected();
    void tryReconnect();

private:
    void flushPendingMessages();
    void enqueueMessage(int code, const QJsonObject &data);

    QWebSocket m_socket;
    QTimer m_reconnectTimer;
    QString m_url;
    QList<PendingMessage> m_pendingMessages;
    bool m_autoReconnect = true;
    bool m_manualDisconnect = false;
    int m_maxReconnectAttempts = 5;
    int m_reconnectAttempts = 0;
};

#endif // GAMEWEBSOCKETCLIENT_H
