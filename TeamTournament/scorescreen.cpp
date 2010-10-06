#include "scorescreen.h"
#include "ui_scorescreen.h"

using namespace Ipponboard;

ScoreScreen::ScoreScreen(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ScoreScreen)
{
	ui->setupUi(this);

	ui->club_blue->SetColor(Qt::white, Qt::blue);
	ui->score_blue->SetColor(Qt::white, Qt::blue);
	ui->logo_blue->SetBgColor(Qt::blue);
	ui->text_dummy1_blue->SetColor(Qt::white, Qt::blue);
	ui->text_dummy2_blue->SetColor(Qt::white, Qt::blue);
	ui->text_dummy1_blue->SetText("");
	ui->text_dummy2_blue->SetText("");

	ui->club_white->SetColor(Qt::black, Qt::white);
	ui->score_white->SetColor(Qt::black, Qt::white);
	ui->logo_white->SetBgColor(Qt::white);
	ui->text_dummy1_white->SetColor(Qt::black, Qt::white);
	ui->text_dummy2_white->SetColor(Qt::black, Qt::white);
	ui->text_dummy1_white->SetText("");
	ui->text_dummy2_white->SetText("");

	SetTextFont( QFont("Arial", 12, QFont::Normal, false ) );
	SetDigitFont( QFont("Arial", 12, QFont::Bold, false ) );

	//ui->text_club_home->setAlignment(Qt::AlignRight);
	//ui->text_club_guest->setAlignment(Qt::AlignLeft);
}

ScoreScreen::~ScoreScreen()
{
	delete ui;
}

//---------------------------------------------------------
void ScoreScreen::SetClubs( const QString& blue, const QString& white )
//---------------------------------------------------------
{
	ui->club_blue->SetText(blue);
	ui->club_white->SetText(white);
}

//---------------------------------------------------------
void ScoreScreen::SetLogos( const QString& fileNameBlue,
							const QString& fileNameWhite )
//---------------------------------------------------------
{
	ui->logo_blue->UpdateImage(fileNameBlue);
	ui->logo_white->UpdateImage(fileNameWhite);
}

//---------------------------------------------------------
void ScoreScreen::SetScore( int scoreBlue, int scoreWhite )
//---------------------------------------------------------
{
	ui->score_blue->SetText( QString::number(scoreBlue) );
	ui->score_white->SetText( QString::number(scoreWhite) );
}

//---------------------------------------------------------
void ScoreScreen::SetTextFont( const QFont& font)
//---------------------------------------------------------
{
	m_textFont = font;

	ui->club_blue->setFont( m_textFont );
	ui->club_white->setFont( m_textFont );
}

//---------------------------------------------------------
void ScoreScreen::SetDigitFont( const QFont& font)
//---------------------------------------------------------
{
	m_digitFont = font;

	ui->score_blue->setFont( m_digitFont );
	ui->score_white->setFont( m_digitFont );
}

//---------------------------------------------------------
void ScoreScreen::changeEvent(QEvent *e)
//---------------------------------------------------------
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}


//---------------------------------------------------------
void ScoreScreen::SetTextColorBlue( const QColor& color, const QColor& bgColor )
//---------------------------------------------------------
{
	ui->logo_blue->SetBgColor(bgColor);
	ui->club_blue->SetColor(color, bgColor);
	ui->score_blue->SetColor(color, bgColor);
	ui->text_dummy1_blue->SetColor(color, bgColor);
	ui->text_dummy2_blue->SetColor(color, bgColor);

	update();
}

//---------------------------------------------------------
void ScoreScreen::SetTextColorWhite( const QColor& color, const QColor& bgColor )
//---------------------------------------------------------
{
	ui->logo_white->SetBgColor(bgColor);
	ui->club_white->SetColor(color, bgColor);
	ui->score_white->SetColor(color, bgColor);
	ui->text_dummy1_white->SetColor(color, bgColor);
	ui->text_dummy2_white->SetColor(color, bgColor);

	update();
}

