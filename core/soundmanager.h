#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QSoundEffect>

class SoundManager : public QObject
{
    Q_OBJECT

public:
    static SoundManager &instance();

    void playDeal();
    void playClick();
    void playWin();
    void playLose();
    void playDiscard();
    void playBomb();
    void playKingBoom();
    void startBackgroundMusic();
    void stopBackgroundMusic();

private:
    SoundManager();

    void playEffect(QSoundEffect &effect, const QString &resourcePath);

    QSoundEffect m_dealEffect;
    QSoundEffect m_clickEffect;
    QSoundEffect m_winEffect;
    QSoundEffect m_loseEffect;
    QSoundEffect m_discardEffect;
    QSoundEffect m_bombEffect;
    QSoundEffect m_kingBoomEffect;
    QSoundEffect m_bgmEffect;
};

#endif // SOUNDMANAGER_H
