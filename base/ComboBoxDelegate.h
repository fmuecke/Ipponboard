// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#ifndef __BASE_COMBOBOXDELEGATE_H
#define __BASE_COMBOBOXDELEGATE_H

#include <QItemDelegate>
#include <QStringList>

// forwards
class QComboBox;

class ComboBoxDelegate : public QItemDelegate
{
public:
    ComboBoxDelegate(QObject *parent);

    QWidget* createEditor(
            QWidget *parent,
            const QStyleOptionViewItem &/* option */,
            const QModelIndex &/* index */) const override;

    void setEditorData(
            QWidget *editor,
            const QModelIndex &index) const  override;

    void setModelData(
            QWidget* editor,
            QAbstractItemModel* model,
            const QModelIndex& index) const override;

    void updateEditorGeometry(
            QWidget* editor,
            const QStyleOptionViewItem& option,
            const QModelIndex&/* index */) const override;

    void SetItems(QStringList const& items);

private:
    QStringList m_items;
};

#endif // __BASE_COMBOBOXDELEGATE_H
