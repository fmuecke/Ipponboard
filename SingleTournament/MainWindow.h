#ifndef BASIC_EDITION_MAINWINDOW_H_
#define BASIC_EDITION_MAINWINDOW_H_

#include "../base/mainwindowbase.h"
#include <boost/shared_ptr.hpp>

class MainWindow : public MainWindowBase
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

	virtual void Init() sealed;

	virtual const char* EditionName() const sealed			{ return "Basic Edition"; }
	virtual const char* EditionNameShort() const sealed		{ return "Basic"; }

protected:
    //virtual void changeEvent(QEvent* e) override;
    //virtual void closeEvent(QCloseEvent* event) override;
    //virtual void keyPressEvent(QKeyEvent* event) override;

private:
	//void update_info_text_color(const QColor& color, const QColor& bgColor) override;
    //void update_text_color_blue(const QColor& color, const QColor& bgColor) override;
    //void update_text_color_white(const QColor& color, const QColor& bgColor) override;
    //virtual void update_fighter_name_font(const QFont&) override;
    //virtual void update_views() override;

	/* specific private methods */
	void update_fighter_name_completer(const QString& weight);
    void update_fighters(const QString& s);
    
	/* base class slot overrides */
	virtual void on_actionImport_Fighters_triggered() override;

protected slots:
    //virtual bool EvaluateSpecificInput(FMlib::Gamepad const* pGamepad) override;

	/* specific private slots */
	void on_actionManage_Classes_triggered();
    void on_comboBox_weight_currentIndexChanged(const QString&);
    void on_comboBox_name_white_currentIndexChanged(const QString&);
    void on_comboBox_name_blue_currentIndexChanged(const QString&);
    void on_checkBox_golden_score_clicked(bool checked);
    void on_comboBox_weight_class_currentIndexChanged(const QString&);
    void on_actionExportList_triggered();

private:
    /* member */
	boost::shared_ptr<Ipponboard::FightCategoryMgr> m_pCategoryManager;
    QStringList m_CurrentFighterNames;
};

#endif  // BASIC_EDITION_MAINWINDOW_H_
