// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef TEAMTOURNAMENT__SCORESCREEN_H_
#define TEAMTOURNAMENT__SCORESCREEN_H_

#include <QWidget>

namespace Ui
{
class ScoreScreen;
}

namespace Ipponboard
{

class ScoreScreen : public QWidget
{
	Q_OBJECT
public:
	explicit ScoreScreen(QWidget* parent = 0);
	~ScoreScreen();

	void SetClubs(const QString& first, const QString& second);
	void SetLogos(const QString& fileNameFirst, const QString& fileNameSecond);
	void SetScore(int scoreFirst, int scoreSecond);
	void SetTextFont(const QFont& font);
	void SetDigitFont(const QFont& font);

	void SetTextColorFirst(const QColor& color, const QColor& bgColor);
	void SetTextColorSecond(const QColor& color, const QColor& bgColor);


protected:
	void changeEvent(QEvent* e);

private:
	Ui::ScoreScreen* ui;
	QFont m_textFont;
	QFont m_digitFont;
};

} // namespace Ipponboard
#endif  // TEAMTOURNAMENT__SCORESCREEN_H_
