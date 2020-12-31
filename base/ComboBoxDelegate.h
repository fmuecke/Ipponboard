// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.


#ifndef __BASE_COMBOBOXDELEGATE_H
#define __BASE_COMBOBOXDELEGATE_H

#include "../util/helpers.hpp"

#include <QItemDelegate>
#include <QStringList>

// forwards
class QComboBox;

class ComboBoxDelegate : public QItemDelegate
{
public:
	ComboBoxDelegate(QObject* parent);

	virtual QWidget* createEditor(
		QWidget* parent,
		const QStyleOptionViewItem& /* option */,
		const QModelIndex& /* index */) const override;

	virtual void setEditorData(
		QWidget* editor,
		const QModelIndex& index) const override;

	virtual void setModelData(
		QWidget* editor,
		QAbstractItemModel* model,
		const QModelIndex& index) const override;

	virtual void updateEditorGeometry(
		QWidget* editor,
		const QStyleOptionViewItem& option,
		const QModelIndex& /* index */) const override;

	void SetItems(QStringList const& items);

private:
	QStringList m_items;
};

#endif // __BASE_COMBOBOXDELEGATE_H
