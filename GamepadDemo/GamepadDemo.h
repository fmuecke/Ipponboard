// Copyright 2018-2025 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef GAMEPADDEMO__GAMEPADDEMO_H_
#define GAMEPADDEMO__GAMEPADDEMO_H_

#include "../gamepad/Gamepad.h"

#include <QMainWindow>
#include <memory>

namespace Ui
{
class GamepadDemo;
}

class GamepadDemo : public QMainWindow
{
    Q_OBJECT
  public:
    GamepadDemo(QWidget* parent = 0);
    ~GamepadDemo();

  protected:
    void changeEvent(QEvent* e);
    void UpdateButtonState(unsigned button) const;
    void UpdateCapabilities();

  private slots:
    void GetData();

  private:
    std::unique_ptr<Ui::GamepadDemo> ui;
    class QLabel* m_pSBarText;
    class QTimer* m_pTimer;
    std::unique_ptr<GamepadLib::Gamepad> m_pGamepad;
};

#endif // GAMEPADDEMO__GAMEPADDEMO_H_