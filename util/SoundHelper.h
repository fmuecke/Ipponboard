// Copyright 2024. All rights reserved.
// Qt6 replacement for QSound using QSoundEffect

#ifndef SOUNDHELPER_H
#define SOUNDHELPER_H

#include <QSoundEffect>
#include <QUrl>
#include <QFileInfo>

namespace Ipponboard {
namespace SoundHelper {

// Simple sound player using QSoundEffect (Qt6 replacement for QSound)
inline void playSound(const QString& filePath) {
    static QSoundEffect* effect = nullptr;
    if (!effect) {
        effect = new QSoundEffect();
        effect->setLoopCount(1);
        effect->setVolume(1.0f);
    }
    
    if (QFileInfo::exists(filePath)) {
        effect->setSource(QUrl::fromLocalFile(filePath));
        effect->play();
    }
}

} // namespace SoundHelper
} // namespace Ipponboard

#endif // SOUNDHELPER_H