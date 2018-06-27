#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Initlize();
}

MainWindow::~MainWindow()
{
    chart->removeAllSeries();
    delete axisY;
    delete axisX;
    delete chart;
    delete chartView;
    delete console;
    delete splitter;
    delete ui;

}

RunnerState MainWindow::getRunState() const
{
    return state;
}

void MainWindow::setRunState(const RunnerState &value)
{
    state = value;
    if(ui)
    {
        if(state == BeforeDecompose)
        {
            ui->action_Open->setEnabled(true);
            ui->action_Run->setEnabled(true);
            ui->action_ReStart->setEnabled(false);
            ui->action_Stop->setEnabled(false);
            ui->action_Clean->setEnabled(true);
        }
        else if(state == StartedDecompose || state == StartedFoam || state == StartedReconstruct)
        {
            ui->action_Open->setEnabled(false);
            ui->action_Run->setEnabled(false);
            ui->action_ReStart->setEnabled(true);
            ui->action_Stop->setEnabled(true);
            ui->action_Clean->setEnabled(false);
        }
        else if(state == FinishedDecompose || state == FinishedFoam || state == FinishedReconstruct)
        {
            ui->action_Open->setEnabled(true);
            ui->action_Run->setEnabled(true);
            ui->action_ReStart->setEnabled(true);
            ui->action_Stop->setEnabled(false);
            ui->action_Clean->setEnabled(true);
        }
    }
}

void MainWindow::resetWorkplace()
{
    Abort = false;
    setRunState(BeforeDecompose);
    chart->removeAllSeries();
    console->clear();
    foamLogText.clear();
}

void MainWindow::Restart()
{
    resetWorkplace();
    setRunState(BeforeDecompose);
    dcRunner.Start();
}

void MainWindow::Initlize()
{
    chartView = new QChartView(this);
    console =new QTextBrowser(this);
    console ->setFontPointSize(10);
    splitter =new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(chartView);
    splitter->addWidget(console);
    splitter->setStretchFactor(0, 9);
    splitter->setStretchFactor(1, 1);
    this->setCentralWidget(splitter);
    Abort = false;
    state = BeforeDecompose;
    chart = new QChart();
    //chart->createDefaultAxes();
    //创建QAxis
    axisX = new QValueAxis();
    axisX->setRange(0, 0.5);
    axisX->setLabelFormat("%.3f"); //设置刻度的格式
    axisX->setGridLineVisible(true);
    axisX->setTickCount(11);     //设置多少格
    axisX->setMinorTickCount(5); //设置每格小刻度线的数目
    axisX->setTitleText("time[s]");

    axisY = new QValueAxis();
    axisY->setRange(-10, 0);
    axisY->setLabelFormat("%.1f"); //设置刻度的格式
    axisY->setGridLineVisible(true);
    axisY->setTickCount(11);     //设置多少格
    axisY->setMinorTickCount(5); //设置每格小刻度线的数目
    axisY->setTitleText("log10(residual  or error)");

    chart->legend()->show();
    chart->legend()->setAlignment(Qt::AlignRight);
    chartView->setChart(chart);

    connect(&dcRunner,SIGNAL(onProcessStart()),this,SLOT(onDecomposeStart()));
    connect(&dcRunner,SIGNAL(onProcessOutput(QString)),this,SLOT(onDecomposeOutput(QString)));
    connect(&dcRunner,SIGNAL(onProcessFinished(int,QProcess::ExitStatus)),this,SLOT(onDecomposeFinished(int,QProcess::ExitStatus)));
    connect(&dcRunner,SIGNAL(onProcessError(QString)),this,SLOT(onDecomposeError(QString)));

    connect(&runner,SIGNAL(onProcessStart()),this,SLOT(onRunnerStart()));
    connect(&runner,SIGNAL(onProcessOutput(QString)),this,SLOT(onRunnerOutput(QString)));
    connect(&runner,SIGNAL(onProcessFinished(int,QProcess::ExitStatus)),this,SLOT(onRunnerFinished(int,QProcess::ExitStatus)));
    connect(&runner,SIGNAL(onProcessError(QString)),this,SLOT(onRunnerError(QString)));

    connect(&rcRunner,SIGNAL(onProcessStart()),this,SLOT(onReconstructStart()));
    connect(&rcRunner,SIGNAL(onProcessOutput(QString)),this,SLOT(onReconstructOutput(QString)));
    connect(&rcRunner,SIGNAL(onProcessFinished(int,QProcess::ExitStatus)),this,SLOT(onReconstructFinished(int,QProcess::ExitStatus)));
    connect(&rcRunner,SIGNAL(onProcessError(QString)),this,SLOT(onReconstructError(QString)));

    connect(&timer,SIGNAL(timeout()),this,SLOT(FoamlogToChart()));

    setRunState(BeforeDecompose);
    ui->action_Open->setIcon(QIcon(":/icons/open"));
    ui->action_Run->setIcon(QIcon(":/icons/start"));
    ui->action_ReStart->setIcon(QIcon(":/icons/restart"));
    ui->action_Clean->setIcon(QIcon(":/icons/clean"));
    ui->action_Stop->setIcon(QIcon(":/icons/stop"));
    ui->action_Quit->setIcon(QIcon(":/icons/quit"));
    ui->actionH_elp->setIcon(QIcon(":/icons/help"));
    ui->action_About->setIcon(QIcon(":/icons/about"));

}
void MainWindow::showAndRun(QString url)
{
    this->show();
    setUrl(url);
    //Restart();
}

void MainWindow::showAndRun()
{
    this->show();
    setUrl(QDir::currentPath());
}

void MainWindow::on_action_Quit_triggered()
{

    this->close();
}

void MainWindow::on_action_Open_triggered()
{
    QStringList    fileNameList;
    QString fileName0;
    QFileDialog* fd = new QFileDialog(this);
    fd->setFileMode(QFileDialog::DirectoryOnly);
    fd->resize(240,320);
    fd->setViewMode(QFileDialog::Detail);
    if ( fd->exec() == QDialog::Accepted )
    {
        fileNameList = fd->selectedFiles();
        fileName0 = fileNameList[0];
        setUrl(fileName0);
        resetWorkplace();
        LogInformation("target:"+fileName0);
    }
    else
        fd->close();
}


void MainWindow::onRunnerStart()
{
    setRunState(StartedFoam);
    LogInformation(QString("%1 start").arg(runner.getSolverName()));
    timer.start(1000);
}

void MainWindow::onRunnerOutput(const QString &output)
{
    foamLogText +=output;
    LogFoam(output);
}

void MainWindow::onRunnerFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    setRunState(FinishedFoam);
    if(exitStatus == QProcess::NormalExit)
    {
        LogInformation("solver stoped normally");
        FoamlogToChart();
        state = FinishedFoam;
        if(!Abort)
            rcRunner.Start();
    }
    else
    {
        LogInformation(QString("solver aborted! exitcode is %1").arg(exitCode));
    }
    timer.stop();
}

void MainWindow::onRunnerError(const QString &value)
{
    LogError(value);
}

void MainWindow::onDecomposeStart()
{
    LogInformation("decomposePar start");
    setRunState(StartedDecompose);
}

void MainWindow::onDecomposeOutput(const QString &output)
{
    LogInformation(output);
}

void MainWindow::onDecomposeError(const QString &value)
{
    LogError(value);
}

void MainWindow::onDecomposeFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    setRunState(FinishedDecompose);
    if(exitStatus == QProcess::NormalExit)
    {

        LogInformation("decomposePar stoped normally");
        if(!Abort)
            runner.Start();
    }
    else
    {
        LogInformation(QString("solver aborted! exitcode is %1").arg(exitCode));
    }
}

void MainWindow::onReconstructStart()
{
    setRunState(StartedReconstruct);
    LogInformation("reconstructPar start");
}

void MainWindow::onReconstructOutput(const QString &output)
{
    LogInformation(output);
}

void MainWindow::onReconstructError(const QString &value)
{
    LogError(value);
}

void MainWindow::onReconstructFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    setRunState(FinishedReconstruct);
    if(exitStatus == QProcess::NormalExit)
    {
        LogInformation("reconstructPar stoped normally");

    }
    else
    {
        LogInformation(QString("solver aborted! exitcode is %1").arg(exitCode));
    }

    CaseCleaner::cleanProcesorDirs(getUrl());
    LogInformation("processror dirs clean finished");
}


QString MainWindow::getUrl() const
{
    return url;
}

void MainWindow::setUrl(const QString &value)
{
    url = value;
    this->setWindowTitle("OpenFOAM Solver UI:"+value);
    dcRunner.setTargetUrl(url);
    runner.setTargetUrl(url);
    rcRunner.setTargetUrl(url);
}

void MainWindow::StartRun()
{
    if(state == StartedFoam||state == FinishedFoam)
    {
        resetWorkplace();
        runner.Start();
    }
    else if(state == StartedReconstruct)
        rcRunner.Start();
    else
        Restart();
}

void MainWindow::StopRun()
{
    Abort = true;
    dcRunner.Stop();
    runner.Stop();
    if(timer.isActive())
        timer.stop();
    rcRunner.Stop();
}

void MainWindow::CleanCase()
{
    CaseCleaner::cleanCase(getUrl());
}

void MainWindow::LogError(const QString &err)
{
    console->setTextColor(QColor(205,16,118));
    console->append(err);
}

void MainWindow::LogInformation(const QString &inf)
{
    console->setTextColor(QColor(54,100,139));
    console->append(inf);
}

void MainWindow::LogFoam(const QString &inf)
{
    console->setTextColor(QColor(54,54,54));
    console->append(inf);
}

void MainWindow::LogError(QStringList &errs)
{
    QStringList::Iterator it = errs.begin();
    QStringList::Iterator iend = errs.end();
    for(;it!=iend;it++)
    {
        LogError(*it);
    }
}

void MainWindow::LogInformation(QStringList &infs)
{
    QStringList::Iterator it = infs.begin();
    QStringList::Iterator iend = infs.end();
    for(;it!=iend;it++)
    {
        LogInformation(*it);
    }
}

void MainWindow::LogFoam(QStringList &infs)
{
    QStringList::Iterator it = infs.begin();
    QStringList::Iterator iend = infs.end();
    for(;it!=iend;it++)
    {
        LogFoam(*it);
    }
}

void MainWindow::FoamlogToChart()
{
    QList<FoamDataPoint> list = decoder.decode(foamLogText);
    QList<QAbstractSeries *> allseries = chart->series();
    foreach (QAbstractSeries *s, allseries) {
        QLineSeries *pls = (QLineSeries *)s;
        pls->clear();
    }
    for(int i=0;i<list.size();i++)
    {
        FoamDataPoint p = list.at(i);
        if(p.getName().endsWith("sum local")||p.getName().endsWith("Initial residual"))
        {
            QLineSeries *series = GetSeries(p.getName());
            double y = 0;
            if(p.getY()>=1e-20)
                y = log10(p.getY());
            double x = p.getX();
            series->append(x,y);
            if(i == 0)
            {
                minX = maxX = x;
                minY = maxY = y;
            }
            else
            {
                minX = qMin(x,minX);
                maxX = qMax(x,maxX);
                minY = qMin(y,minY);
                maxY = qMax(y,maxY);
            }
            qDebug() << p.getName();
            //qDebug("p.x:%f  p.y:%f  ",p.getX(),p.getY());
            qDebug("  x:%f    y:%f  ",x,y);
        }

    }
    axisX->setRange(minX,maxX);
    axisY->setRange(minY,maxY);
}

QLineSeries *MainWindow::GetSeries(const QString &name)
{
    QLineSeries *rs = NULL;
    QList<QAbstractSeries *> series = chart->series();
    for(int i=0;i<series.size();i++)
    {
        QAbstractSeries *cs = series.at(i);
        if(cs->name() == name)
                rs = (QLineSeries *)cs;
    }
    if(rs == NULL)
    {
        rs = new QLineSeries();
        rs->setName(name);
        QColor qc=QColor::fromHsl(rand()%360,rand()%256,rand()%200);
        rs->setPen(QPen(qc,2,Qt::SolidLine));

        chart->addSeries(rs);
        chart->setAxisX(axisX,rs);
        chart->setAxisY(axisY,rs);

    }
    return rs;
}

void MainWindow::on_action_Run_triggered()
{
    if(dcRunner.isRunning()||runner.isRunning()||rcRunner.isRunning())
    {
        QMessageBox::information(this,"information","Simulation is running!",QMessageBox::Ok);
    }
    else
        StartRun();
}

void MainWindow::on_action_Stop_triggered()
{
    if(dcRunner.isRunning()||runner.isRunning()||rcRunner.isRunning())
    {
        int res = QMessageBox::question(this,"question","Stop simulation?",QMessageBox::Yes|QMessageBox::No);
        if(res == QMessageBox::Yes)
            StopRun();
    }
    else
        StopRun();
}

void MainWindow::on_action_ReStart_triggered()
{
    if(dcRunner.isRunning()||runner.isRunning()||rcRunner.isRunning())
    {
        int res = QMessageBox::question(this,"question","Restart simulation?",QMessageBox::Yes|QMessageBox::No);
        if(res == QMessageBox::Yes)
        {
            StopRun();
            Restart();
        }
    }
    else
    {
        if(state !=BeforeDecompose)
        {
            int res = QMessageBox::question(this,"question","Simulation was processed,Restart it?",QMessageBox::Yes|QMessageBox::No);
            if(res == QMessageBox::Yes)
                Restart();
        }
        else
            Restart();
    }

}


void MainWindow::closeEvent(QCloseEvent *event)
{
    if(dcRunner.isRunning()||runner.isRunning()||rcRunner.isRunning())
    {
        int res = QMessageBox::question(this,"question","Simulation is running,Stop it and quit?",QMessageBox::Yes|QMessageBox::No);
        if(res == QMessageBox::Yes)
        {
            StopRun();
            event->accept();
        }
        else
            event->ignore();
    }
    else
        event->accept();
}

void MainWindow::on_action_Clean_triggered()
{
    CaseCleaner::cleanCase(getUrl());
    LogInformation("case clean finished!");
}

void MainWindow::on_actiontest_triggered()
{

}
