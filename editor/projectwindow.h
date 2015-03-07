#ifndef PROJECTWINDOW_H
#define PROJECTWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTimer>

#include "baseproject.h"
#include "languagehighlighter.h"

namespace Ui {
class ProjectWindow;
}

class ProjectWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProjectWindow(QWidget *parent = 0, QString projectDir = QString());
    ~ProjectWindow();

public slots:
    void updateMenus();
    void setEditorText(QString code);
    void saveProject();
    void openOptionsDialog();
    void updateCodeAnalysis();

private slots:
    void build();
    void flash();
    void run(bool pressed);

    void programStopped();

    void setTargetFromMenu();

    void printConsoleText(QString text);
    void printConsoleError(QString text);

private:
//    void createMenus();
    void connectActions();

    void readSettings();
    void writeSettings();

    Ui::ProjectWindow *ui;
    QWidget *m_layoutContainer;
//    QToolBar *m_toolBar;

//    BaseProject *m_project;
    QFile m_codeFile;
    LanguageHighlighter *m_highlighter;

    QString m_projectDir;
    QString m_platformsRootDir;
    QMap<QString, QVariant> m_options;
    QTimer m_timer;
};

#endif // PROJECTWINDOW_H
