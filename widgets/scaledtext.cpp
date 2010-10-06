#include <QtGui>

#include "scaledtext.h"

ScaledText::ScaledText( QWidget* pParent )
	: QWidget(pParent)
	, m_Text()
	, m_TextColor(Qt::color1)  // normal foreground
	, m_BGColor(Qt::transparent)  // normal background (transparent)
	, m_Alignment(Qt::AlignCenter)
	, m_pLayout(0)
	, m_timerId(-1)
	, m_timerDelay(0)
	, m_isDigitSize(false)
	, m_isVisible(true)
	, m_rotate(false)
{
	SetText( "ScaledText" );
}

//ScaledText::ScaledText( const QFont& font, QWidget* pParent )
//	: QWidget(pParent)
//	, m_Text()
//	, m_TextColor(Qt::color1)  // normal foreground
//	, m_BGColor(Qt::transparent)  // normal background (transparent)
//	, m_Alignment(Qt::AlignCenter)
//	, m_pLayout(0)
//	, m_timerId(-1)
//	, m_timerDelay(0)
//	, m_isDigitSize(false)
//	, m_isVisible(true)
//	, m_rotate(false)
//{
//	this->setFont( font );
//	SetText( "ScaledText" );
//}


void ScaledText::SetText( const QString& text, bool rotate )
{
	m_Text = text;
	m_rotate = rotate;

	const QFont& currentFont = this->font();

	delete m_pLayout;
	//font.setPointSize(32);
	m_pLayout = new QTextLayout(text, currentFont);
	m_pLayout->setCacheEnabled(true);
//	QTextOption option(Qt::AlignLeft);
//	option.setUseDesignMetrics(true);
//	m_pLayout->setTextOption(option);
	m_pLayout->beginLayout();
	m_pLayout->createLine();
	m_pLayout->endLayout();

	update();
}

void ScaledText::SetFont( const QFont &font )
{
    this->setFont(font);
    Redraw();
}

void ScaledText::SetFontAndColor( const QFont& font, const QColor& textColor,
					  const QColor& bgColor )
{
	this->setFont(font);
	SetColor( textColor, bgColor );
    Redraw();
}


void ScaledText::SetColor( const QColor& textColor, const QColor& bgColor )
{
	m_TextColor = textColor;
	m_BGColor = bgColor;

	update();
}

void ScaledText::SetColor( const QColor& textColor)
{
	m_TextColor = textColor;

	update();
}

const QColor& ScaledText::GetColor() const
{
	return m_TextColor;
}

const QColor& ScaledText::GetBgColor() const
{
	return m_BGColor;
}

void ScaledText::SetDigitSize( bool digitSize )
{
	m_isDigitSize = digitSize;
}

bool ScaledText::IsDigitSize() const
{
	return m_isDigitSize;
}

void ScaledText::SetBlinking( bool blink, int delay )
{
	if( blink && -1 != m_timerId && delay == m_timerDelay )
		return;

	// kill old timer
	if( -1 != m_timerId )
		killTimer(m_timerId);
	m_timerId = -1;
	m_timerDelay = delay;
	m_isVisible = true;

	if( blink )
		m_timerId = startTimer(delay);
}

bool ScaledText::IsBlinking() const
{
	return (m_timerId != -1);
}

void ScaledText::Redraw()
{
    SetText(m_Text);
}

void ScaledText::paintEvent( QPaintEvent* event )
{
	QPainter painter(this);
	painter.save();
	painter.fillRect( event->rect(), QBrush(m_BGColor) );
	if( m_isVisible )
	{
		painter.setRenderHint( QPainter::TextAntialiasing );
		if (m_pLayout != 0)
		{
			painter.setPen( m_TextColor );
			painter.translate(width() / 2, height() / 2);

			QTextLine line = m_pLayout->lineAt(0);
			line.setLeadingIncluded(false);
			qreal w = m_rotate ?
					  line.naturalTextRect().height() :
					  line.naturalTextRect().width();
			qreal h = m_rotate ?
					  line.naturalTextRect().width() :
					  line.naturalTextRect().height();
			if( m_isDigitSize && !m_rotate )
				h -= line.descent()*2;
			qreal zoom_x = width()/w;
			qreal zoom_y = height()/h;
			zoom_x = std::min<qreal>(zoom_x,zoom_y);
			zoom_y = zoom_x;
			if( m_rotate )
				painter.rotate(-60.0);
			painter.scale(zoom_x, zoom_y);

			QPointF center = line.naturalTextRect().center();
			if( Qt::AlignLeft == m_Alignment )
				center.setX(width()/2./zoom_x);
			else if( Qt::AlignRight == m_Alignment )
				center.setX(-width()/2./zoom_x + center.x()*2);
			line.draw(&painter, -center);
		}
	}
	painter.restore();
	painter.end();
}

void ScaledText::timerEvent( QTimerEvent* /*event*/ )
{
	m_isVisible = !m_isVisible;

	update();
}
