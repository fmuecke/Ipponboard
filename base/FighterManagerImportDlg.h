// Copyright 2010-2014 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#ifndef FIGHTERMANAGERIMPORTDLG_H
#define FIGHTERMANAGERIMPORTDLG_H

#include <QDialog>

namespace Ui {
class FighterManagerImportDlg;
}

class FighterManagerImportDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FighterManagerImportDlg(QString const& filePath, QStringList const& clubs, QWidget *parent = 0);
    ~FighterManagerImportDlg();

    QString GetFormatStr() const;
    QString GetSelectedClub() const;

private slots:
    void on_toolButton_firstname_toggled(bool checked);
    void on_toolButton_lastname_toggled(bool checked);
    void on_toolButton_nation_toggled(bool checked);
    void on_toolButton_year_toggled(bool checked);
    void on_toolButton_club_toggled(bool checked);
    //void on_toolButton_team_toggled(bool checked);
    void on_toolButton_category_toggled(bool checked);
    void on_toolButton_weight_toggled(bool checked);
    void on_comboBox_separator_currentIndexChanged(const QString &s);
    void on_lineEdit_specifiers_textChanged(const QString& s);
    void on_checkBox_importToClub_toggled(bool checked);

private:
    void ToggleSpecifier(QString const& specifier, bool checked);

    enum EColumns
    {
        eColumn_Lastname = 0,
        eColumn_Firstname,
        eColumn_Club,
        //eColumn_Team,
        eColumn_Year,
        eColumn_Nation,
        eColumn_Weight,
        eColumn_Category
    };

    Ui::FighterManagerImportDlg *ui;
    QString m_parseFormat;
    QStringList m_separators;
    QString m_currentSeparator;
};

#endif // FIGHTERMANAGERIMPORTDLG_H
