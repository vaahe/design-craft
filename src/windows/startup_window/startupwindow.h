#ifndef STARTUPWINDOW_H
#define STARTUPWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QListWidget>

class StartupWindow : public QWidget {
    Q_OBJECT

public:
    explicit StartupWindow(QWidget *parent = nullptr);

signals:
    void openProjectRequested();
    void createProjectRequested();
    void projectSelected(const QString projectName);

private slots:
    void handleCreateProject();
    void handleOpenProject();
    void handleProjectSelection();

private:
    QListWidget *recentProjectsList;
};

#endif // STARTUPWINDOW_H
