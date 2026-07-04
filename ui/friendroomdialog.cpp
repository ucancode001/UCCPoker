#include "friendroomdialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRandomGenerator>
#include <QVBoxLayout>

FriendRoomDialog::FriendRoomDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("好友房间"));
    setMinimumWidth(420);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(QStringLiteral("创建房间邀请好友，或输入房间号加入"), this));

    auto *createButton = new QPushButton(QStringLiteral("创建新房间"), this);
    createButton->setStyleSheet(
        "QPushButton { background:#43A047; color:white; border:none; border-radius:18px; padding:10px; font-weight:bold; }");
    connect(createButton, &QPushButton::clicked, this, [this]() {
        m_action = Action::CreateRoom;
        const QString roomId = QString::number(QRandomGenerator::global()->bounded(100000, 999999));
        m_roomEdit->setText(roomId);
        accept();
    });
    layout->addWidget(createButton);

    auto *form = new QFormLayout;
    m_roomEdit = new QLineEdit(this);
    m_roomEdit->setPlaceholderText(QStringLiteral("输入 6 位房间号"));
    m_roomEdit->setMaxLength(12);
    form->addRow(QStringLiteral("房间号"), m_roomEdit);
    layout->addLayout(form);

    auto *joinButton = new QPushButton(QStringLiteral("加入房间"), this);
    joinButton->setStyleSheet(
        "QPushButton { background:#1976D2; color:white; border:none; border-radius:18px; padding:10px; font-weight:bold; }");
    connect(joinButton, &QPushButton::clicked, this, [this]() {
        m_action = Action::JoinRoom;
        accept();
    });
    layout->addWidget(joinButton);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);
}

QString FriendRoomDialog::roomId() const
{
    return m_roomEdit->text().trimmed();
}
