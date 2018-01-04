#include "mainwindow.h"
#include "dbconnection.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "connect.h"
#include <QMessageBox>
#include <QFile>
#include <QSettings>
#include <QTextStream>
#include <QFileInfo>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settings=new Settings(this);
    connectWindow=new Connect(this);

    createActions();

    readSettings();

    connect(ui->textEditRAEditor->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    setCurrentFile("");
    setUnifiedTitleAndToolBarOnMac(true);
    ui->treeRelations->setHeaderLabel(tr("Relations"));
    runPending=false;
    setupEditor(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupEditor(bool clear_editor=false)
{
    QFont font;
    font.setFamily(FONT_FAMILY);
    font.setFixedPitch(true);
    font.setPointSize(FONT_SIZE);

    if (clear_editor)
        ui->textEditRAEditor->clear();
    ui->textEditRAEditor->setFont(font);
    highlighter = new Highlighter(ui->textEditRAEditor->document());
    sqlQueryHighlighter = new Highlighter(ui->textEditSqlQuery->document());
    richRAParser = new RichRAParser(ui->textEditRAEditor);
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::createActions()
{
    connect(ui->action_Exit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->action_New, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(ui->action_Settings, SIGNAL(triggered()), this, SLOT(displaySettings()));
    connect(ui->action_Connect, SIGNAL(triggered()), this, SLOT(displayConnect()));


    QStandardItemModel model(4, 4);
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            QStandardItem *item = new QStandardItem(QString("row %0, column %1").arg(row).arg(column));
            model.setItem(row, column, item);
        }
    }

    //ui->listView_relations->setModel(&model);
}

void MainWindow::documentWasModified()
{
    setWindowModified(ui->textEditRAEditor->document()->isModified());
}

/*******************************************************/
/* Menu Actions                                        */
/*******************************************************/

void MainWindow::about()

{
    QMessageBox::about(this, tr("About RAT"),
                       tr("RAT (Relational Algebra Translator) V0.1.\nDeveloped by: Mangesh and Nilam."));
}


void MainWindow::on_action_About_triggered()
{
    about();
}



//****************************************************
// Settings
//****************************************************

void MainWindow::readSettings()
{
    QSettings settings(COMPANY_NAME, APP_NAME);
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings(COMPANY_NAME, APP_NAME);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

//****************************************************
// Open Activities
//****************************************************

void MainWindow::newFile()
{
    if (maybeSave()) {
        ui->textEditRAEditor->clear();
        setCurrentFile("");
    }
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ui->textEditRAEditor->setHtml(in.readAll());;
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}


void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Rat files (*.rat)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}
void MainWindow::on_action_Open_triggered()
{
    open();
}

bool MainWindow::maybeSave()
{
    if (ui->textEditRAEditor->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                                   tr("The document has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}


//****************************************************
// Save Activities
//****************************************************

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    ui->textEditRAEditor->document()->setModified(false);
    setWindowModified(false);;

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.rat";
    setWindowFilePath(shownName);

}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    //out << ui->textEditRAEditor->toPlainText();
    if(fileName.right(4) == ".txt")
    {
        QString plainText, sqlQuery, errorText;
        richRAParser->parseRA(plainText, sqlQuery, errorText);
        out << plainText;
    }
    else
    {
        out << ui->textEditRAEditor->toHtml();
    }
    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}


bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "untitled.rat", tr("Rat files (*.rat);;Plain Text files(*.txt)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void MainWindow::on_action_Save_triggered()
{
    save();
}

void MainWindow::on_actionSave_As_triggered()
{
    saveAs();
}

/*******************************************************/
/* Connection Settings                                 */
/*******************************************************/

void MainWindow::displaySettings()
{
    settings->show();
}

void MainWindow::displayConnect()
{
    connectWindow->show();
    if (DBConnection::connected)
    {
        populateRelations();
        statusBar()->showMessage(tr("Connected to Database"), 2000);
    }
}

/*******************************************************/
/* Actions                                             */
/*******************************************************/

int MainWindow::parseOutput(QString &sqlQuery)
{

    int ret = richRAParser->parseRA(plainText, sqlQuery, errorText);
    //QMessageBox::information(this,APP_NAME,sqlQuery);
    //ui->tableView_Results->setModel(null);
    ui->textEditSqlQuery->clear();
    if (!ret)
    {
        // Success
        //Display sqlQuery in sqlQuery window
        ui->textEditSqlQuery->insertPlainText(sqlQuery + "\n");

        //Display plaintext in trace window
        ui->textEditTrace->append("Plain Text RA Query:: ");
        ui->textEditTrace->append(plainText + "\n");
        ui->textEditTrace->append("Parsing to SQL Query successful. Please check SQL Query tab\n\n");
        ui->textEditTrace->ensureCursorVisible();
        ui->tabWidget->setCurrentIndex(1);
    }
    ui->textEditSqlQuery->ensureCursorVisible();
    ui->textEditSqlQuery->setFocus();
    return ret;
}

void MainWindow::displayRAError()
{
    //Display plaintext in trace window
    ui->textEditTrace->append("Plain Text RA Query::\n");
    ui->textEditTrace->append(plainText + "\n");

  //  QMessageBox::information(this,QString(APP_NAME) + "::Parsing Error", errorText);
    ui->textEditTrace->append("Parsing Error:: ");
    ui->textEditTrace->append(errorText + "\n\n");

    ui->textEditTrace->ensureCursorVisible();
    ui->tabWidget->setCurrentIndex(2);
}


void MainWindow::on_action_Parse_triggered()
{
    QString sqlQuery;

    int ret=parseOutput(sqlQuery);
    if (ret)
        displayRAError();

}


void MainWindow::on_action_Run_triggered()
{
    QString sqlQuery;
    QStringList sqlQueryList;
    QStringList::const_iterator constIterator;
    bool ok=false;
    int ret = parseOutput(sqlQuery);


    if (!DBConnection::connected)
    {
        displayConnect();
        runPending=true;
        return;
    }

    //if not validate schema clear it
    if (!settings->ValidateSchema)
        richRAParser->clearSchema();

    // display the output in dataGrid
    ui->tableView_Results->setModel(DBConnection::model);

    //Run the queries
    sqlQueryList = sqlQuery.split(QRegExp(";"));
    for (constIterator = sqlQueryList.constBegin(); constIterator != sqlQueryList.constEnd();
    ++constIterator)
    {
     ok = DBConnection::query->exec(*constIterator);
     if(ok)
         continue;
     else
         break;
    }


    if (ok)
    {
        if (DBConnection::querySizeFeature)
            statusBar()->showMessage(QString::number(DBConnection::query->size()) + tr(" rows selected"));
        DBConnection::model->setQuery(*DBConnection::query);
        ui->tabWidget->setCurrentIndex(0);
        ui->tableView_Results->resizeColumnsToContents();

        if (sqlQuery.contains("TABLE", Qt::CaseInsensitive))
        {
            DBConnection::retrieveAttrs();
            populateRelations();
        }

    }
    else if (ret)
        displayRAError();
    else
    {

        //Error from DBMS
        QString dbError = DBConnection::query->lastError().text();

        //QMessageBox::information(this,QString(APP_NAME) + "::DB Error", dbError);
        ui->textEditTrace->append("DB Error:: ");
        ui->textEditTrace->append(dbError + "\n\n");
        ui->tabWidget->setCurrentIndex(2);
    }

}


void MainWindow::populateRelations()
{
    QStringList tableList = DBConnection::db.tables();

    ui->treeRelations->clear();

    richRAParser->clearSchema();

    QStringList::const_iterator constIterator;

    for (constIterator = tableList.constBegin(); constIterator != tableList.constEnd();
    ++constIterator)
    {
        list <wstring> attributes;

        QTreeWidgetItem *tableItem = new QTreeWidgetItem();
        tableItem->setText(0, (*constIterator));
        //tableItem->addChildren(new QTreeWidgetItem("Hello"));;
        ui->treeRelations->addTopLevelItem(tableItem);
        QStringList fieldList = DBConnection::fields.fieldNames(*constIterator);
        QStringList::const_iterator fieldsIterator;
        for (fieldsIterator = fieldList.constBegin(); fieldsIterator != fieldList.constEnd();
        ++fieldsIterator)
        {
            QTreeWidgetItem *fieldItem = new QTreeWidgetItem();
            fieldItem->setText(0, (*fieldsIterator));
            tableItem->addChild(fieldItem);
            attributes.push_back((*fieldsIterator).toStdWString());
        }
        if(settings->ValidateSchema)
            richRAParser->updateSchema((*constIterator).toStdWString(), attributes);
    }
    setupEditor(false);
    if (runPending)
    {
        runPending=false;
        on_action_Run_triggered();
    }
}

/*******************************************************/
/* Operator Actions                                    */
/*******************************************************/

void MainWindow::addTextEditSQL(const QString &oper, int typeOper,  QString suffix = "condition"){

    //QString htmlFont = APP_FONT;
    QString htmlFont = "";
    QString htmlStringUn, htmlStringUn1;
    QString htmlStringBin, htmlStringBin1, htmlStringBin2, htmlStringBinSuf;

    QTextCursor cursor = ui->textEditRAEditor->textCursor();
    int countOfSelected=1, selectionStart=1;

    htmlStringUn = "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"  + htmlFont;
    htmlStringUn1 = "<sub>  " + suffix + " </sub>(Relation)</span></p>";

    htmlStringBin = "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">" + htmlFont + "Relation1 ";
    htmlStringBin1 ="</span>";
    htmlStringBinSuf = "<sub> " + suffix + " </sub>";
    htmlStringBin2 =  " Relation2 </span></p>";

    switch (typeOper){
    case 1:
        ui->textEditRAEditor->insertHtml(htmlStringUn + oper + htmlStringUn1);
        if(suffix == "newRelation (columns)")
            selectionStart = 7;
        else
            selectionStart = 4;

        break;
    case 2:
        ui->textEditRAEditor->insertHtml(htmlStringBin + oper + htmlStringBin2);
        selectionStart = 3;
        break;
    case 3:
        ui->textEditRAEditor->insertHtml(htmlStringBin + oper + htmlStringBinSuf + htmlStringBin2);
        selectionStart = 4;
        break;

    }

    cursor.movePosition(QTextCursor::PreviousWord,QTextCursor::MoveAnchor, selectionStart );
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor, countOfSelected );
    ui->textEditRAEditor->setTextCursor (cursor);

    ui->textEditRAEditor->setFocus();

}

void MainWindow::on_pushButton_oper_select_clicked()
{
    QString oper = "&sigma;";//σ";
    //QString ab = "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Sans'; font-size:10pt;\">select * from customer</span><span style=\" font-family:'Sans'; font-size:10pt; vertical-align:sub;\">abc</span></p>";
    addTextEditSQL(oper, 1);
}

void MainWindow::on_pushButton_oper_project_clicked()
{
    QString oper = "&pi;"; //#960
    addTextEditSQL(oper, 1, "columNames");

}

void MainWindow::on_pushButton_oper_union_clicked()
{
    QString oper = "&cup;";//U "&cup;"; ";
    addTextEditSQL(oper, 2);
}


void MainWindow::on_pushButton_oper_intersection_clicked()
{
    QString oper = "&#8745;";//∩";
    addTextEditSQL(oper, 2);
}

void MainWindow::on_pushButton_oper_cartestian_clicked()
{
    QString oper = "&#215;";  // ×
    addTextEditSQL(oper, 2);
}

void MainWindow::on_pushButton_oper_natural_clicked()
{
    QString oper = "&#8904;";   //&#10781;";//⋈";
    addTextEditSQL(oper, 2);
}


void MainWindow::on_pushButton_oper_theta_clicked()
{
    QString oper = "&#8904;";   //&#10781;";//⋈";
    addTextEditSQL(oper, 3);
}

void MainWindow::on_pushButton_oper_thetaouterjoin_clicked()
{
    QString oper = "&#10199;";
    addTextEditSQL(oper, 3);
}

void MainWindow::on_pushButton_oper_thetarightouterjoin_clicked()
{
    QString oper = "&#10198;";
    addTextEditSQL(oper, 3);
}

void MainWindow::on_pushButton_oper_thetaleftouterjoin_clicked()
{
    QString oper = "&#10197;";
    addTextEditSQL(oper, 3);
}

void MainWindow::on_pushButton_oper_outerjoin_clicked()
{
    QString oper = "&#10199;";
    addTextEditSQL(oper, 2);
}

void MainWindow::on_pushButton_oper_leftouterjoin_clicked()
{
    QString oper = "&#10197;";
    addTextEditSQL(oper, 2);
}

void MainWindow::on_pushButton_oper_rightouterjoin_clicked()
{
    QString oper = "&#10198;";
    addTextEditSQL(oper, 2);
}


void MainWindow::on_pushButton_oper_rename_clicked()
{
    QString oper = "&#961;";   //"&rho;"
    addTextEditSQL(oper, 1, "newRelation (columns)");
}

void MainWindow::on_pushButton_difference_clicked()
{
    QString oper = "&#8722;";
    addTextEditSQL(oper, 2);
}

void MainWindow::on_pushButton_oper_assign_clicked()
{
    QString oper = "&#8592;";
    addTextEditSQL(oper, 2);
}


/*******************************************************/
/* Relation Tree Actions                               */
/*******************************************************/

void MainWindow::on_treeRelations_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
    ui->textEditRAEditor->insertPlainText(item->text(column));
    ui->textEditRAEditor->setFocus();
}



void MainWindow::on_action_Undo_triggered()
{
    ui->textEditRAEditor->undo();
}

void MainWindow::on_action_Redo_triggered()
{
    ui->textEditRAEditor->redo();
}


void MainWindow::on_actionZoom_In_triggered()
{
    ui->textEditRAEditor->zoomIn();

}

void MainWindow::on_actionZoom_Out_triggered()
{
    ui->textEditRAEditor->zoomOut();
    QMessageBox msgBox;
    msgBox.setText( QApplication::focusWidget()->metaObject()->className());
    msgBox.exec();

}

void MainWindow::on_actionActual_Size_triggered()
{

}
