// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#include "ComboBoxDelegate.h"

#include <QComboBox>

ComboBoxDelegate::ComboBoxDelegate(QObject* parent)
	: QItemDelegate(parent)
{
}

QWidget* ComboBoxDelegate::createEditor(
	QWidget* parent,
	const QStyleOptionViewItem& /*option*/,
	const QModelIndex& index) const
{
	QComboBox* editor = new QComboBox(parent);
	//editor->setEditable(false);
	editor->addItems(m_items);
	editor->setParent(parent);

	// set index to currently selected item
	QString currentData = index.model()->data(index, Qt::DisplayRole).toString();

	if (!currentData.isEmpty())
	{
		int index = 0;

		for (; index < m_items.size(); ++index)
		{
			if (m_items[index] == currentData)
			{
				editor->setCurrentIndex(index);
				break;
			}
		}
	}

	return editor;
}

void ComboBoxDelegate::setEditorData(
	QWidget* /*editor*/,
	const QModelIndex& /*index*/) const
{
	//QString value = index.model()->data(index, Qt::EditRole).toString();
	//QComboBox *comboBox = static_cast<QComboBox*>(editor);
}

void ComboBoxDelegate::setModelData(
	QWidget* editor,
	QAbstractItemModel* model,
	const QModelIndex& index) const
{
	QComboBox* comboBox = static_cast<QComboBox*>(editor);
	model->setData(index, comboBox->currentText(), Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(
	QWidget* editor,
	const QStyleOptionViewItem& option,
	const QModelIndex& /*index*/) const
{
	editor->setGeometry(option.rect);
}

void ComboBoxDelegate::SetItems(QStringList const& items)
{
	m_items = items;
}

__BASE_COMBOBOXDELEGATE_H
