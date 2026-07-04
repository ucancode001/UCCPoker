#include "settingsdialog.h"
#include "core/appsettings.h"
#include "network/gamewebsocketclient.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("游戏设置"));
    setMinimumWidth(460);

    auto *layout = new QVBoxLayout(this);
    auto *form = new QFormLayout;

    m_nameEdit = new QLineEdit(AppSettings::instance().playerName(), this);
    m_serverEdit = new QLineEdit(AppSettings::instance().serverUrl(), this);
    m_soundCheck = new QCheckBox(QStringLiteral("启用音效"), this);
    m_soundCheck->setChecked(AppSettings::instance().soundEnabled());
    m_fullscreenCheck = new QCheckBox(QStringLiteral("全屏模式"), this);
    m_fullscreenCheck->setChecked(AppSettings::instance().fullscreen());
    m_supersampleCombo = new QComboBox(this);
    m_supersampleCombo->addItem(QStringLiteral("标准 (1x)"), 1);
    m_supersampleCombo->addItem(QStringLiteral("高清 (2x 超采样)"), 2);
    m_supersampleCombo->addItem(QStringLiteral("4K (3x 超采样)"), 3);
    const int ssIndex = m_supersampleCombo->findData(AppSettings::instance().supersampleFactor());
    m_supersampleCombo->setCurrentIndex(ssIndex >= 0 ? ssIndex : 0);
    m_reconnectCheck = new QCheckBox(QStringLiteral("断线自动重连"), this);
    m_reconnectCheck->setChecked(AppSettings::instance().autoReconnectEnabled());
    m_reconnectSpin = new QSpinBox(this);
    m_reconnectSpin->setRange(1, 10);
    m_reconnectSpin->setValue(AppSettings::instance().reconnectMaxAttempts());

    form->addRow(QStringLiteral("昵称"), m_nameEdit);
    form->addRow(QStringLiteral("服务器"), m_serverEdit);
    form->addRow(QString(), m_soundCheck);
    form->addRow(QString(), m_fullscreenCheck);
    form->addRow(QStringLiteral("渲染质量"), m_supersampleCombo);
    form->addRow(QString(), m_reconnectCheck);
    form->addRow(QStringLiteral("重连次数"), m_reconnectSpin);
    layout->addLayout(form);

    auto *testRow = new QHBoxLayout;
    auto *testButton = new QPushButton(QStringLiteral("测试连接"), this);
    m_testStatusLabel = new QLabel(this);
    m_testStatusLabel->setStyleSheet("color: #78909C; font-size: 13px;");
    testRow->addWidget(testButton);
    testRow->addWidget(m_testStatusLabel, 1);
    layout->addLayout(testRow);
    connect(testButton, &QPushButton::clicked, this, &SettingsDialog::onTestConnection);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &SettingsDialog::onSave);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

void SettingsDialog::onSave()
{
    AppSettings::instance().setPlayerName(m_nameEdit->text());
    AppSettings::instance().setServerUrl(m_serverEdit->text());
    AppSettings::instance().setSoundEnabled(m_soundCheck->isChecked());
    AppSettings::instance().setFullscreen(m_fullscreenCheck->isChecked());
    AppSettings::instance().setSupersampleFactor(m_supersampleCombo->currentData().toInt());
    AppSettings::instance().setAutoReconnectEnabled(m_reconnectCheck->isChecked());
    AppSettings::instance().setReconnectMaxAttempts(m_reconnectSpin->value());
    emit settingsSaved();
    accept();
}

void SettingsDialog::onTestConnection()
{
    m_testStatusLabel->setText(QStringLiteral("正在连接..."));
    m_testStatusLabel->setStyleSheet("color: #FFB74D; font-size: 13px;");

    if (m_testClient) {
        m_testClient->disconnectFromServer();
        m_testClient->deleteLater();
    }

    m_testClient = new GameWebSocketClient(this);
    m_testClient->setAutoReconnect(false);

    connect(m_testClient, &GameWebSocketClient::connected, this, [this]() {
        m_testStatusLabel->setText(QStringLiteral("连接成功"));
        m_testStatusLabel->setStyleSheet("color: #66BB6A; font-size: 13px;");
        QTimer::singleShot(800, this, [this]() {
            if (m_testClient)
                m_testClient->disconnectFromServer();
        });
    });
    connect(m_testClient, &GameWebSocketClient::connectionError, this, [this](const QString &message) {
        m_testStatusLabel->setText(QStringLiteral("连接失败：%1").arg(message));
        m_testStatusLabel->setStyleSheet("color: #EF5350; font-size: 13px;");
    });

    const QString url = AppSettings::instance().buildWebSocketUrlForName(m_nameEdit->text().trimmed(),
                                                                         m_serverEdit->text().trimmed());
    m_testClient->connectToServer(url);
}
