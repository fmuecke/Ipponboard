#ifndef WIDGETS__SCALEDIMAGE_H_
#define WIDGETS__SCALEDIMAGE_H_
// Copyright 2010-2014 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
// $Id$

#include <QtGui/QWidget>
#include <QImage>
#include <QResizeEvent>

class ScaledImage : public QWidget
{
	Q_OBJECT

public:
	explicit ScaledImage(QWidget* parent = 0);

	void UpdateImage(const QString& fileName);
	void SetBgColor(const QColor& color);
	void paintEvent(QPaintEvent* pEvent);
	void resizeEvent(QResizeEvent* pEvent);

private: //Data
	QImage m_Image;
	QSize m_Size;
	QColor m_BGColor;
};

#endif  // WIDGETS__SCALEDIMAGE_H_
