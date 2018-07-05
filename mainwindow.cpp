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

FoamState MainWindow::getRunnerState() const
{
    return runnerState;
}

void MainWindow::setRunnerState(const FoamState &value)
{
    runnerState = value;
    /*if(ui)
    {
        if(runnerState == BeforeDecompose)
        {
            ui->action_Open->setEnabled(true);
            ui->action_Run->setEnabled(true);
            //ui->action_ReStart->setEnabled(false);
            ui->action_Stop->setEnabled(false);
            ui->action_Clean->setEnabled(true);
            ui->action_Decompose->setEnabled(true);
            ui->action_Reconstruct->setEnabled(false);
        }
        else if(runnerState == StartedDecompose || runnerState == StartedFoam || runnerState == StartedReconstruct)
        {
            ui->action_Open->setEnabled(false);
            ui->action_Run->setEnabled(false);
            ui->action_ReStart->setEnabled(true);
            ui->action_Stop->setEnabled(true);
            ui->action_Clean->setEnabled(false);
        }
        else if(runnerState == FinishedDecompose || runnerState == FinishedFoam || runnerState == FinishedReconstruct)
        {
            ui->action_Open->setEnabled(true);
            ui->action_Run->setEnabled(true);
            ui->action_ReStart->setEnabled(true);
            ui->action_Stop->setEnabled(false);
            ui->action_Clean->setEnabled(true);
        }
    }*/
    updateActionsState();
    writeFoamState();
}

void MainWindow::resetWorkplace()
{
    Abort = false;
    chart->removeAllSeries();
    console->clear();
    foamLogText.clear();
}

void MainWindow::readFoamState()
{
    QFile f(getUrl()+QDir::separator()+".foamstate");
    if(!f.open(QIODevice::ReadOnly))
    {
        decomposeState = BeforeDecompose;
        runnerState = BeforeFoam;
        reconstructState = BeforeReconstruct;
        updateActionsState();
        return;
    }
    else
    {
        QTextStream s(&f);
        QString data = s.readLine();
        QStringList ds = data.split(' ');
        if(ds.length()!=3)
        {
            decomposeState = BeforeDecompose;
            runnerState = BeforeFoam;
            reconstructState = BeforeReconstruct;
            updateActionsState();
        }
        else
        {
            QString dcs = ds[0];
            QString rns = ds[1];
            QString rcs = ds[2];
            int ics = dcs.toInt();
            int irn = rns.toInt();
            int irc = rcs.toInt();
            decomposeState = (FoamState)ics;
            runnerState = (FoamState)irn;
            reconstructState = (FoamState)irc;
            updateActionsState();
        }

        f.close();
    }
}

void MainWindow::writeFoamState()
{
    QFile f(getUrl()+QDir::separator()+".foamstate");
    if(!f.open(QIODevice::WriteOnly))
    {
        return;
    }
    QTextStream s(&f);
    int dcs = (int)getDecomposeState();
    int rns = (int)getRunnerState();
    int rcs = (int)getReconstructState();
    s << (QString("%1 %2 %3").arg(dcs).arg(rns).arg(rcs));
    f.close();
}

FoamState MainWindow::getDecomposeState() const
{
    return decomposeState;

}

void MainWindow::setDecomposeState(const FoamState &value)
{
    decomposeState = value;
    updateActionsState();
    writeFoamState();
}

FoamState MainWindow::getReconstructState() const
{
    return reconstructState;
}

void MainWindow::setReconstructState(const FoamState &value)
{
    reconstructState = value;
    updateActionsState();
    writeFoamState();
}

void MainWindow::updateActionsState()
{
    if(getDecomposeState()==StartedDecompose||getRunnerState()==StartedFoam||getReconstructState()==StartedReconstruct)
        DisableAllFoamActionButStop();
    else
        EnbaleAllFoamActionButStop();
    if(getDecomposeState()==FinishedDecompose&&getRunnerState()==FinishedFoam)
        ui->action_Reconstruct->setEnabled(true);
    else
        ui->action_Reconstruct->setEnabled(false);
}

void MainWindow::Restart()
{
    resetWorkplace();
    setRunnerState(BeforeDecompose);
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
    runnerState = BeforeDecompose;
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

    setRunnerState(BeforeDecompose);
    ui->action_Open->setIcon(QIcon(":/icons/open"));
    ui->action_Run->setIcon(QIcon(":/icons/start"));
    ui->action_ReStart->setIcon(QIcon(":/icons/restart"));
    ui->action_Clean->setIcon(QIcon(":/icons/clean"));
    ui->action_Stop->setIcon(QIcon(":/icons/stop"));
    ui->action_Quit->setIcon(QIcon(":/icons/quit"));
    ui->actionH_elp->setIcon(QIcon(":/icons/help"));
    ui->action_About->setIcon(QIcon(":/icons/about"));
    ui->action_Decompose->setIcon(QIcon(":/icons/decompose"));
    ui->action_Reconstruct->setIcon(QIcon(":/icons/reconstruct"));
    ui->action_Initlize->setIcon(QIcon(":/icons/initialize"));

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
        LogInformation("target:"+fileName0);
    }
    else
        fd->close();
}


void MainWindow::onRunnerStart()
{
    setRunnerState(StartedFoam);
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
    setRunnerState(FinishedFoam);
    if(exitStatus == QProcess::NormalExit)
    {
        LogInformation("solver stoped normally");
        FoamlogToChart();
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
    setDecomposeState(StartedDecompose);
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

    if(exitStatus == QProcess::NormalExit)
    {
        LogInformation("decomposePar stoped normally");
        setDecomposeState(FinishedDecompose);
    }
    else
    {
        LogInformation(QString("solver aborted! exitcode is %1").arg(exitCode));
        setDecomposeState(BeforeDecompose);
    }
}

void MainWindow::onReconstructStart()
{
    setReconstructState(StartedReconstruct);
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

    if(exitStatus == QProcess::NormalExit)
    {
        LogInformation("reconstructPar stoped normally");
        setReconstructState(FinishedReconstruct);
    }
    else
    {
        LogInformation(QString("solver aborted! exitcode is %1").arg(exitCode));
        setReconstructState(BeforeReconstruct);
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
    resetWorkplace();
    dcRunner.setTargetUrl(url);
    runner.setTargetUrl(url);
    rcRunner.setTargetUrl(url);
    readFoamState();
}

void MainWindow::StartRun()
{
    if(runnerState == StartedFoam||runnerState == FinishedFoam)
    {
        resetWorkplace();
        runner.Start();
    }
    else if(runnerState == StartedReconstruct)
        rcRunner.Start();
    else
        Restart();
}

void MainWindow::StopAllRun()
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
    console->setTextColor(QColor(54,100,139));
#if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
    if(err.endsWith("\r\n"))
        console->append(err.left(err.length()-2));
    else
        console->append(err);
#else
    if(err.endsWith("\n"))
        console->append(err.left(err.length()-1));
    else
        console->append(err);
#endif


}

void MainWindow::LogInformation(const QString &inf)
{
    console->setTextColor(QColor(54,100,139));
#if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
    if(inf.endsWith("\r\n"))
        console->append(inf.left(inf.length()-2));
    else
        console->append(inf);
#else
    if(inf.endsWith("\n"))
        console->append(inf.left(inf.length()-1));
    else
        console->append(inf);
#endif
}

void MainWindow::LogFoam(const QString &inf)
{
    console->setTextColor(QColor(54,100,139));
#if defined(Q_OS_WIN32)||defined(Q_OS_WIN64)
    if(inf.endsWith("\r\n"))
        console->append(inf.left(inf.length()-2));
    else
        console->append(inf);
#else
    if(inf.endsWith("\n"))
        console->append(inf.left(inf.length()-1));
    else
        console->append(inf);
#endif
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

void MainWindow::DisableAllFoamActionButStop()
{
    if(ui)
    {
        ui->action_Clean->setEnabled(false);
        ui->action_Decompose->setEnabled(false);
        ui->action_Initlize->setEnabled(false);
        ui->action_Open->setEnabled(false);
        ui->action_Reconstruct->setEnabled(false);
        ui->action_Run->setEnabled(false);
        ui->action_Stop->setEnabled(true);
    }
}

void MainWindow::EnbaleAllFoamActionButStop()
{
    if(ui)
    {
        ui->action_Clean->setEnabled(true);
        ui->action_Decompose->setEnabled(true);
        ui->action_Initlize->setEnabled(true);
        ui->action_Open->setEnabled(true);
        ui->action_Reconstruct->setEnabled(true);
        ui->action_Run->setEnabled(true);
        ui->action_Stop->setEnabled(false);
    }
}

void MainWindow::FoamlogToChart()
{
    QFile f(runner.getLogfileUrl());
    if(f.open(QFile::ReadOnly))
    {
        QTextStream ts(&f);
        QString data = ts.readAll();
        QList<FoamDataPoint> list = decoder.decode(data);
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
        f.close();
    }

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
    runner.Start();
}

void MainWindow::on_action_Stop_triggered()
{
    if(runner.isRunning())
    {
        int res = QMessageBox::question(this,"question","Stop simulation?",QMessageBox::Yes|QMessageBox::No);
        if(res == QMessageBox::Yes)
        {
            runner.Stop();
            if(timer.isActive())
                timer.stop();
        }
        else
            return;
    }
    else if(dcRunner.isRunning()||rcRunner.isRunning())
    {
        int res = QMessageBox::question(this,"question","Stop Decompose or Reconstruct?",QMessageBox::Yes|QMessageBox::No);
        if(res == QMessageBox::Yes)
        {
            dcRunner.Stop();
            rcRunner.Stop();
        }
    }

}

void MainWindow::on_action_ReStart_triggered()
{
    if(dcRunner.isRunning()||runner.isRunning()||rcRunner.isRunning())
    {
        int res = QMessageBox::question(this,"question","Restart simulation?",QMessageBox::Yes|QMessageBox::No);
        if(res == QMessageBox::Yes)
        {
            StopAllRun();
            Restart();
        }
    }
    else
    {
        if(runnerState !=BeforeDecompose)
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
            StopAllRun();
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
    setDecomposeState(BeforeDecompose);
    setRunnerState(BeforeFoam);
    setReconstructState(BeforeReconstruct);
    LogInformation("case clean finished!");
}

void MainWindow::on_actiontest_triggered()
{

}

void MainWindow::on_action_Decompose_triggered()
{
    dcRunner.Start();
}

void MainWindow::on_action_Reconstruct_triggered()
{
    rcRunner.Start();
}

void MainWindow::on_action_Initlize_triggered()
{
    CaseCleaner::cleanTimeDirectories(getUrl());
    setRunnerState(BeforeFoam);
    setReconstructState(BeforeReconstruct);
}
