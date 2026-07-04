#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>

class AppSettings
{
public:
    static AppSettings &instance();

    QString playerName() const;
    void setPlayerName(const QString &name);

    QString serverUrl() const;
    void setServerUrl(const QString &url);

    bool soundEnabled() const;
    void setSoundEnabled(bool enabled);

    bool fullscreen() const;
    void setFullscreen(bool enabled);

    int supersampleFactor() const;
    void setSupersampleFactor(int factor);

    bool autoReconnectEnabled() const;
    void setAutoReconnectEnabled(bool enabled);

    int reconnectMaxAttempts() const;
    void setReconnectMaxAttempts(int attempts);

    QString lastRoomId() const;
    void setLastRoomId(const QString &roomId);

    QString buildWebSocketUrl() const;
    static QString buildWebSocketUrlForName(const QString &playerName, const QString &serverUrl);

private:
    AppSettings();
    void load();
    void save();

    QString m_playerName;
    QString m_serverUrl;
    QString m_lastRoomId;
    bool m_soundEnabled = true;
    bool m_fullscreen = false;
    int m_supersampleFactor = 1;
    bool m_autoReconnect = true;
    int m_reconnectMaxAttempts = 5;
};

#endif // APPSETTINGS_H
