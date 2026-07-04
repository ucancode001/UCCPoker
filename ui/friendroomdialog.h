#ifndef FRIENDROOMDIALOG_H
#define FRIENDROOMDIALOG_H

#include <QDialog>

class QLineEdit;

class FriendRoomDialog : public QDialog
{
    Q_OBJECT

public:
    enum class Action {
        CreateRoom,
        JoinRoom
    };

    explicit FriendRoomDialog(QWidget *parent = nullptr);

    Action selectedAction() const { return m_action; }
    QString roomId() const;

private:
    Action m_action = Action::CreateRoom;
    QLineEdit *m_roomEdit = nullptr;
};

#endif // FRIENDROOMDIALOG_H
