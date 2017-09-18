#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>

#include "settings.h"
#include "connect.h"
#include "highlighter.h"
#include "richraparser.h"

#define APP_NAME "RAT"
#define COMPANY_NAME "2 Idiots"
#define FONT_FAMILY "Sans Serif"
#define FONT_SIZE 15
#define APP_FONT "<span style=\" font-family:'Sans Serif'; font-size:15pt;\">"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /* Other Forms */
    Settings *settings;
    Connect *connectWindow;
    void populateRelations();

protected:
    void changeEvent(QEvent *e);
   // void closeEvent(QCloseEvent *event);

private slots:

    void on_pushButton_oper_assign_clicked();
    void on_pushButton_oper_thetaleftouterjoin_clicked();
    void on_pushButton_oper_thetarightouterjoin_clicked();
    void on_pushButton_oper_thetaouterjoin_clicked();
    void on_action_Redo_triggered();
    void on_action_Undo_triggered();
    void on_actionSave_As_triggered();
    void on_action_Save_triggered();
    void on_action_Open_triggered();
    void on_action_About_triggered();
    void on_pushButton_difference_clicked();
    void on_pushButton_oper_rightouterjoin_clicked();
    void on_pushButton_oper_leftouterjoin_clicked();
    void on_pushButton_oper_outerjoin_clicked();
    void on_pushButton_oper_natural_clicked();
    void on_action_Parse_triggered();
    void on_treeRelations_itemDoubleClicked(QTreeWidgetItem* item, int column);
    void on_pushButton_oper_rename_clicked();
    void on_pushButton_oper_theta_clicked();
    void on_pushButton_oper_cartestian_clicked();
    void on_pushButton_oper_intersection_clicked();
    void on_pushButton_oper_project_clicked();
    void on_pushButton_oper_union_clicked();
    void on_pushButton_oper_select_clicked();
    void on_action_Run_triggered();
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();
    void displaySettings();
    void displayConnect();

private:
    void createActions();
    bool maybeSave();

    int parseOutput(QString &sqlQuery);
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void addTextEditSQL(const QString &oper, int numOfOper, QString suffix);
    QString strippedName(const QString &fullFileName);

    void displayRAError();

    QString curFile;
    Highlighter *highlighter;
    Highlighter *sqlQueryHighlighter;
    RichRAParser *richRAParser;

    void setupEditor(bool);
    void readSettings();
    void writeSettings();
    bool runPending;

    QString plainText, errorText;


    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
