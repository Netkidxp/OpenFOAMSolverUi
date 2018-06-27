#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "foamrunner.h"
#include "decomposerunner.h"
#include "reconstrucerunner.h"

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QTextBrowser>
#include <QSplitter>
#include <QFileSystemWatcher>
#include <QFileDialog>
#include <QDebug>
#include <QColor>
#include <QPalette>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "foamdatapoint.h"
#include "foamlogdecoder.h"
#include <QValueAxis>
#include <math.h>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>
#include "casecleaner.h"
#include <QMutex>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MainWindow;
}


typedef enum
{
    BeforeDecompose,
    StartedDecompose,
    FinishedDecompose,
    StartedFoam,
    FinishedFoam,
    StartedReconstruct,
    FinishedReconstruct
}RunnerState;


class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QString url;
    Ui::MainWindow *ui;
    QChartView *chartView;
    QTextBrowser *console;
    QSplitter *splitter;
    FoamRunner runner;
    DecomposeRunner dcRunner;
    ReconstruceRunner rcRunner;
    QChart *chart;
    QValueAxis *axisX;
    QValueAxis *axisY;
    bool Abort;
    RunnerState state;
    FoamLogDecoder decoder;
    QString foamLogText;
    double minX,maxX,minY,maxY;
    QTimer timer;
    QMutex mutex;
    void Restart();
    void Initlize();
    void StartRun();
    void StopRun();
    void CleanCase();
    void LogError(const QString &err);
    void LogInformation(const QString &inf);
    void LogFoam(const QString &inf);
    void LogError(QStringList &errs);
    void LogInformation(QStringList &infs);
    void LogFoam(QStringList &infs);

    QLineSeries *GetSeries(const QString &name);
    RunnerState getRunState() const;
    void setRunState(const RunnerState &value);
    void resetWorkplace();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void showAndRun(QString url);
    void showAndRun();
    QString getUrl() const;
    void setUrl(const QString &value);



private slots:
    void on_action_Quit_triggered();
    void on_action_Open_triggered();

    void onRunnerStart();
    void onRunnerOutput(const QString &output);
    void onRunnerFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onRunnerError(const QString &value);

    void onDecomposeStart();
    void onDecomposeOutput(const QString &output);
    void onDecomposeError(const QString &value);
    void onDecomposeFinished(int exitCode, QProcess::ExitStatus exitStatus);

    void onReconstructStart();
    void onReconstructOutput(const QString &output);
    void onReconstructError(const QString &value);
    void onReconstructFinished(int exitCode, QProcess::ExitStatus exitStatus);

    void FoamlogToChart();

    void on_action_Run_triggered();
    void on_action_Stop_triggered();
    void on_action_ReStart_triggered();

    // QWidget interface
    void on_action_Clean_triggered();

    void on_actiontest_triggered();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
