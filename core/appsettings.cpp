#include "appsettings.h"

#include <QSettings>
#include <QUrl>

AppSettings &AppSettings::instance()
{
    static AppSettings settings;
    return settings;
}

AppSettings::AppSettings()
{
    load();
}

void AppSettings::load()
{
    QSettings settings(QStringLiteral("UCCPocker"), QStringLiteral("UCCPocker"));
    m_playerName = settings.value(QStringLiteral("player/name"), QStringLiteral("玩家")).toString();
    m_serverUrl = settings.value(QStringLiteral("network/serverUrl"),
                                 QStringLiteral("ws://132.232.2.225:8800/pokerWebSocket")).toString();
    m_lastRoomId = settings.value(QStringLiteral("network/lastRoomId")).toString();
    m_soundEnabled = settings.value(QStringLiteral("audio/enabled"), true).toBool();
    m_fullscreen = settings.value(QStringLiteral("display/fullscreen"), false).toBool();
    m_supersampleFactor = qBound(1, settings.value(QStringLiteral("display/supersample"), 1).toInt(), 3);
    m_autoReconnect = settings.value(QStringLiteral("network/autoReconnect"), true).toBool();
    m_reconnectMaxAttempts = settings.value(QStringLiteral("network/reconnectMaxAttempts"), 5).toInt();
}

void AppSettings::save()
{
    QSettings settings(QStringLiteral("UCCPocker"), QStringLiteral("UCCPocker"));
    settings.setValue(QStringLiteral("player/name"), m_playerName);
    settings.setValue(QStringLiteral("network/serverUrl"), m_serverUrl);
    settings.setValue(QStringLiteral("network/lastRoomId"), m_lastRoomId);
    settings.setValue(QStringLiteral("audio/enabled"), m_soundEnabled);
    settings.setValue(QStringLiteral("display/fullscreen"), m_fullscreen);
    settings.setValue(QStringLiteral("display/supersample"), m_supersampleFactor);
    settings.setValue(QStringLiteral("network/autoReconnect"), m_autoReconnect);
    settings.setValue(QStringLiteral("network/reconnectMaxAttempts"), m_reconnectMaxAttempts);
}

QString AppSettings::playerName() const { return m_playerName; }

void AppSettings::setPlayerName(const QString &name)
{
    m_playerName = name.trimmed().isEmpty() ? QStringLiteral("玩家") : name.trimmed();
    save();
}

QString AppSettings::serverUrl() const { return m_serverUrl; }

void AppSettings::setServerUrl(const QString &url)
{
    m_serverUrl = url.trimmed();
    save();
}

bool AppSettings::soundEnabled() const { return m_soundEnabled; }

void AppSettings::setSoundEnabled(bool enabled)
{
    m_soundEnabled = enabled;
    save();
}

bool AppSettings::fullscreen() const { return m_fullscreen; }

void AppSettings::setFullscreen(bool enabled)
{
    m_fullscreen = enabled;
    save();
}

int AppSettings::supersampleFactor() const { return m_supersampleFactor; }

void AppSettings::setSupersampleFactor(int factor)
{
    m_supersampleFactor = qBound(1, factor, 3);
    save();
}

bool AppSettings::autoReconnectEnabled() const { return m_autoReconnect; }

void AppSettings::setAutoReconnectEnabled(bool enabled)
{
    m_autoReconnect = enabled;
    save();
}

int AppSettings::reconnectMaxAttempts() const { return m_reconnectMaxAttempts; }

void AppSettings::setReconnectMaxAttempts(int attempts)
{
    m_reconnectMaxAttempts = qMax(1, attempts);
    save();
}

QString AppSettings::lastRoomId() const { return m_lastRoomId; }

void AppSettings::setLastRoomId(const QString &roomId)
{
    m_lastRoomId = roomId.trimmed();
    save();
}

QString AppSettings::buildWebSocketUrl() const
{
    return buildWebSocketUrlForName(m_playerName, m_serverUrl);
}

QString AppSettings::buildWebSocketUrlForName(const QString &playerName, const QString &serverUrl)
{
    QString url = serverUrl.trimmed();
    if (!url.endsWith(QLatin1Char('/')))
        url += QLatin1Char('/');
    const QString name = playerName.trimmed().isEmpty() ? QStringLiteral("玩家") : playerName.trimmed();
    url += QUrl::toPercentEncoding(name);
    return url;
}
