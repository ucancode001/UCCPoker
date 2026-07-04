#include "soundmanager.h"
#include "appsettings.h"

SoundManager &SoundManager::instance()
{
    static SoundManager manager;
    return manager;
}

SoundManager::SoundManager()
{
    playEffect(m_dealEffect, QStringLiteral(":/image/audio/puker_deal.wav"));
    playEffect(m_clickEffect, QStringLiteral(":/image/audio/click.wav"));
    playEffect(m_winEffect, QStringLiteral(":/image/audio/win.wav"));
    playEffect(m_loseEffect, QStringLiteral(":/image/audio/lose.wav"));
    playEffect(m_discardEffect, QStringLiteral(":/image/audio/click.wav"));
    playEffect(m_bombEffect, QStringLiteral(":/image/audio/boom_bomb.wav"));
    playEffect(m_kingBoomEffect, QStringLiteral(":/image/audio/boom_king.wav"));
    m_bombEffect.setVolume(0.62f);
    m_kingBoomEffect.setVolume(0.72f);
    playEffect(m_bgmEffect, QStringLiteral(":/image/audio/bg_guzheng.mp3"));
    m_bgmEffect.setLoopCount(QSoundEffect::Infinite);
    m_bgmEffect.setVolume(0.12f);
}

void SoundManager::playEffect(QSoundEffect &effect, const QString &resourcePath)
{
    effect.setSource(QUrl(resourcePath));
    effect.setVolume(0.35f);
}

void SoundManager::playDeal()
{
    if (AppSettings::instance().soundEnabled())
        m_dealEffect.play();
}

void SoundManager::playClick()
{
    if (AppSettings::instance().soundEnabled())
        m_clickEffect.play();
}

void SoundManager::playWin()
{
    if (AppSettings::instance().soundEnabled())
        m_winEffect.play();
}

void SoundManager::playLose()
{
    if (AppSettings::instance().soundEnabled())
        m_loseEffect.play();
}

void SoundManager::playDiscard()
{
    if (AppSettings::instance().soundEnabled())
        m_discardEffect.play();
}

void SoundManager::playBomb()
{
    if (AppSettings::instance().soundEnabled())
        m_bombEffect.play();
}

void SoundManager::playKingBoom()
{
    if (AppSettings::instance().soundEnabled())
        m_kingBoomEffect.play();
}

void SoundManager::startBackgroundMusic()
{
    if (AppSettings::instance().soundEnabled() && m_bgmEffect.status() == QSoundEffect::Ready)
        m_bgmEffect.play();
}

void SoundManager::stopBackgroundMusic()
{
    m_bgmEffect.stop();
}
