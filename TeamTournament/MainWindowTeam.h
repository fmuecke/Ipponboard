#ifndef TEAM_EDITION_MAINWINDOW_H_
#define TEAM_EDITION_MAINWINDOW_H_

#include "../base/MainWindowBase.h"
#include "../core/Fighter.h"
#include "../core/TournamentMode.h"
#include "../util/helpers.hpp"

#include <boost/shared_ptr.hpp>

class QTableView;
class QPrinter;
class QSettings;
class QString;
class QPoint;

class MainWindowTeam : public MainWindowBase
{
	Q_OBJECT
public:
	explicit MainWindowTeam(QWidget* parent = nullptr);
	virtual ~MainWindowTeam() {}

	virtual void Init() final;

	virtual const char* EditionName() const final		{ return "Team Edition"; }
	virtual const char* EditionNameShort() const final	{ return "Team"; }

	void SetModes(std::vector<Ipponboard::TournamentMode>& modes)
	{
		m_modes.swap(modes);
	}

protected:
	//virtual void changeEvent(QEvent* e) override;
	virtual void closeEvent(QCloseEvent* event) override;
	virtual void keyPressEvent(QKeyEvent* event) override;

private:
	void update_info_text_color(const QColor& color, const QColor& bgColor) override;
	void update_text_color_first(const QColor& color, const QColor& bgColor) override;
	void update_text_color_second(const QColor& color, const QColor& bgColor) override;
	virtual void update_fighter_name_font(const QFont&) override;
	virtual void update_views() override;

	// specific methods
	void update_club_views();
	void UpdateFightNumber_();
	void UpdateButtonText_();
	void update_score_screen();
	void WriteScoreToHtml_();
	virtual void write_specific_settings(QSettings& settings) final;
	virtual void read_specific_settings(QSettings& settings) final;
	//void update_fighter_name_completer(const QString& weight);
	//void update_fighters(const QString& s);

	/* base class slot overrides */
	virtual void on_actionManageFighters_triggered() override;

private slots:
	void on_tableView_tournament_list1_customContextMenuRequested(QPoint const& pos);
	void on_tableView_tournament_list2_customContextMenuRequested(QPoint const& pos);
	void on_actionScore_Control_triggered();
	void on_actionScore_Screen_triggered();
	void on_button_pause_clicked();
	void on_actionSet_Round_Time_triggered();
	void on_pushButton_copySwitched_pressed();
	void on_toolButton_weights_pressed();
	void on_toolButton_team_home_pressed();
	void on_toolButton_team_guest_pressed();
	void slot_copy_cell_content_list1();
	void slot_copy_cell_content_list2();
	void slot_paste_cell_content_list1();
	void slot_paste_cell_content_list2();
	void slot_clear_cell_content_list1();
	void slot_clear_cell_content_list2();
	void Print(QPrinter* p);
	void on_tabWidget_currentChanged(int index);
	void on_actionManage_Clubs_triggered();
	void on_actionLoad_Demo_Data_triggered();
	void on_button_prev_clicked();
	void on_button_next_clicked();
	void on_comboBox_mode_currentIndexChanged(const QString& s);
	void on_comboBox_club_host_currentIndexChanged(const QString& s);
	void on_comboBox_club_home_currentIndexChanged(const QString& s);
	void on_comboBox_club_guest_currentIndexChanged(const QString& s);
	void on_actionPrint_triggered();
	void on_actionExport_triggered();

	//void on_actionManage_Classes_triggered();
	//void on_comboBox_weight_currentIndexChanged(const QString&);
	//void on_comboBox_name_second_currentIndexChanged(const QString&);
	//void on_comboBox_name_first_currentIndexChanged(const QString&);
	//void on_checkBox_golden_score_clicked(bool checked);
	//void on_comboBox_weight_class_currentIndexChanged(const QString&);

	void on_actionReset_Scores_triggered();
	virtual bool EvaluateSpecificInput(FMlib::Gamepad const* pGamepad) override;

private:
	void update_weights(QString const& weightString);
	void on_tableView_customContextMenuRequested(QTableView* pTableView,
			QPoint const& pos,
			const char* copySlot,
			const char* pasteSlot,
			const char* clearSlot);
	void copy_cell_content(QTableView* pTableView);
	void paste_cell_content(QTableView* pTableView);
	void clear_cell_content(QTableView* pTableView);
	QString get_template_file(QString const& mode) const;
	QString get_full_mode_title(QString const& mode) const;

	boost::shared_ptr<Ipponboard::ScoreScreen> m_pScoreScreen;
	boost::shared_ptr<Ipponboard::ClubManager> m_pClubManager;
	QString m_htmlScore;
	QString m_mode;
	QString m_host;

	//boost::shared_ptr<Ipponboard::FightCategoryMgr> m_pCategoryManager;
	QStringList m_FighterNamesHome;
	QStringList m_FighterNamesGuest;
	std::vector<Ipponboard::TournamentMode> m_modes;
};

#endif  // TEAM_EDITION_MAINWINDOW_H_
