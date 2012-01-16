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

	void SetClubs(const QString& blue, const QString& white);
	void SetLogos(const QString& fileNameBlue, const QString& fileNameWhite);
	void SetScore(int scoreBlue, int scoreWhite);
	void SetTextFont(const QFont& font);
	void SetDigitFont(const QFont& font);

	void SetTextColorBlue(const QColor& color, const QColor& bgColor);
	void SetTextColorWhite(const QColor& color, const QColor& bgColor);


protected:
	void changeEvent(QEvent* e);

private:
	Ui::ScoreScreen* ui;
	QFont m_textFont;
	QFont m_digitFont;
};

} // namespace Ipponboard
#endif  // TEAMTOURNAMENT__SCORESCREEN_H_
