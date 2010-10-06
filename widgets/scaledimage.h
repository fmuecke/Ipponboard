#ifndef SCALEDIMAGE_H
#define SCALEDIMAGE_H

#include <QtGui/QWidget>
#include <QImage>
#include <QResizeEvent>

class ScaledImage : public QWidget
{
	Q_OBJECT

public:
	ScaledImage(QWidget *parent = 0);

	void UpdateImage( const QString& fileName );
	void SetBgColor( const QColor& color );
	void paintEvent(QPaintEvent*);
	void resizeEvent(QResizeEvent*);

private: //Data
	QImage m_Image;
	QSize m_Size;
	QColor m_BGColor;
};

#endif
