#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

class GameWebSocketClient;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QSpinBox;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

signals:
    void settingsSaved();

private slots:
    void onSave();
    void onTestConnection();

private:
    QLineEdit *m_nameEdit = nullptr;
    QLineEdit *m_serverEdit = nullptr;
    QCheckBox *m_soundCheck = nullptr;
    QCheckBox *m_fullscreenCheck = nullptr;
    QComboBox *m_supersampleCombo = nullptr;
    QCheckBox *m_reconnectCheck = nullptr;
    QSpinBox *m_reconnectSpin = nullptr;
    QLabel *m_testStatusLabel = nullptr;
    GameWebSocketClient *m_testClient = nullptr;
};

#endif // SETTINGSDIALOG_H
