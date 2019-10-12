#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "init_dialog.h"

#include<cmath>
#include<cstdlib>

#include<string>
#include<QString>

#include <QToolBar>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , pt(nullptr)
    , current_max_PID(0)
{

//set up
    ui->setupUi(this);

//fixed size
    setFixedSize(820,600);

    ui->page_view->horizontalHeader()->setResizeContentsPrecision(QHeaderView::Stretch);

    connect(ui->actionconfig,&QAction::triggered,this,&MainWindow::init);
    connect(ui->actionclear,&QAction::triggered,this,&MainWindow::clean);

//tool bar
/*
    QToolBar * tb = addToolBar(tr("&Reset"));
    QAction * qa = new QAction(QIcon(":/img/logo1.png"),tr("reset"));
    //connect(qa,&QAction::triggered,this,&da);
    connect(qa,&QAction::triggered,this,&MainWindow::init);
    tb->addAction(qa);
    */

//page view
/*
    auto page_view = ui->page_view;
    page_view->setRowCount(3);
    page_view->setItem(4,4,new QTableWidgetItem("aaaa"));
*/

//config widget
    ind = new init_dialog(this);
    ind->setModal(true);

    init();

}

void MainWindow::init()
{

    if(ind->exec()==QDialog::Rejected){
        if(pt) return;
        else {
            exit(0);
        }
    }

    //some work to reflash
    if(pt) delete pt;


    //use hasher1 as default
    Hasher * hasher_p;
    if(ind->get_hash_name()=="hash_1"){
        hasher_p = new Hasher_1();
    }
    //init pt

    men_size_of_power = ind->get_men_size_of_power();
    page_size_of_power = ind->get_page_size_of_power();

    pt = new PageTable((unsigned short)pow(2,ind->get_men_size_of_power() ),
                       (unsigned short)pow(2,ind->get_page_size_of_power() ));
    pt->set_hasher(hasher_p);

    //set the show board
    ui->men_size_label->setNum(pow(2,ind->get_men_size_of_power()));
    ui->page_size_label->setNum(pow(2,ind->get_page_size_of_power()));
    ui->page_count_label->setNum((int)pt->get_page_count());
    reflash_table();
}

void MainWindow::reflash_table(){
    entrys_info esi(pt);
    reflash_table_by_entrys_info(esi);
}

void MainWindow::reflash_table_by_process(const Process &p){
    entrys_info esi(pt,p);

    qDebug()<<esi.size;

    reflash_table_by_entrys_info(esi);
}

void MainWindow::reflash_table_by_entrys_info(const entrys_info &esi)
{
    ui->page_view->setRowCount(esi.size);

    for(unsigned int i=0;i<esi.size;i++)
    {
        entry_info* ei = esi.entrys[i];
        ui->page_view->setItem(i,0,new QTableWidgetItem
        (QString("%1").arg(ei->count, 22-page_size_of_power, 2, QLatin1Char('0'))));
        //(QString::number(ei->count)));
        //(QString("%1").arg(ei->count, 32, 2, QLatin1Char('0'))));
        ui->page_view->setItem(i,1,new QTableWidgetItem(QString::number(ei->PID)));
        ui->page_view->setItem(i,2,new QTableWidgetItem(QString::number(ei->page_id)));
        ui->page_view->setItem(i,3,new QTableWidgetItem(ei->if_possess? "false":"true"));
        ui->page_view->setItem(i,4,new QTableWidgetItem(ei->if_dirty? "true":"false"));
    }
}

void MainWindow::clean()
{
    if(pt)
        pt->clean();
    reflash_table();
}

void MainWindow::log(const char info[])
{

    ui->logWidget->textCursor().insertText(info);
    ui->logWidget->textCursor().insertText("\r");
    ui->logWidget->moveCursor(QTextCursor::End);
}

MainWindow::~MainWindow(){
    delete ui;
    delete ind;
    if(pt) delete pt;
}




void MainWindow::on_random_create_button_clicked()
{
    int total_size = ui->random_total_size->text().toInt()*1024*1024;
    int min_size   = ui->random_min_size->text().toInt();
    int max_size   = ui->random_max_size->text().toInt();

    if(min_size>max_size||max_size>total_size){
        log("arg error");
        return;
    }

    if(running_processes.size()!=0){
        pt->clean();
        running_processes.clear();
        current_max_PID = 0;
    }

    const int wave = max_size - min_size;
    int left = total_size - min_size;

    int tem = min_size<wave? min_size:wave;

    while(left>tem){
        int random_size = rand()%wave+min_size;
        if (random_size>left) random_size = left;
        left -= random_size;
        Process p(current_max_PID++,random_size);

        QString loginfo = "add process: "+QString::number(current_max_PID-1)+" size: "+QString::number(random_size);
        log(loginfo.toLatin1().data());

        pt->add_process(p);
        running_processes.push_back(p);
    }
    Process p(current_max_PID++,left+min_size);
    pt->add_process(p);
    running_processes.push_back(p);

    QString loginfo = "add process: "+QString::number(current_max_PID-1)+" size: "+QString::number(left+min_size);
    log(loginfo.toLatin1().data());

    reflash_table();
}

void MainWindow::on_random_add_button_clicked()
{
    int total_size = ui->random_total_size->text().toInt()*1024*1024;
    int min_size   = ui->random_min_size->text().toInt();
    int max_size   = ui->random_max_size->text().toInt();


    if(min_size>max_size||max_size>total_size){
        log("arg error");
        return;
    }


    const int wave = max_size - min_size;
    int left = total_size - min_size;

    int tem = min_size<wave? min_size:wave;

    while(left>tem){
        int random_size = rand()%wave+min_size;
        if (random_size>left) random_size = left;
        left -= random_size;
        Process p(current_max_PID++,random_size);
        pt->add_process(p);
        running_processes.push_back(p);

        QString loginfo = "add process: "+QString::number(current_max_PID-1)+" size: "+QString::number(random_size);
        log(loginfo.toLatin1().data());

    }
    Process p(current_max_PID++,left+min_size);
    pt->add_process(p);
    running_processes.push_back(p);

    QString loginfo = "add process: "+QString::number(current_max_PID-1)+" size: "+QString::number(left+min_size);
    log(loginfo.toLatin1().data());

    reflash_table();
}

void MainWindow::on_process_show_clicked()
{
   int PID = ui->process_id->text().toInt();
   reflash_table_by_process(running_processes[PID]);
}

void MainWindow::on_process_run_clicked()
{
   int PID = ui->process_id->text().toInt();
   pt->run_process(running_processes[PID]);
   reflash_table_by_process(running_processes[PID]);
}

void MainWindow::on_page_number_button_clicked()
{
    int PID = ui->process_id->text().toInt();
    int page_id = ui->page_id->text().toInt();

    int page_number = pt->get_page_id(PID,page_id);

    QString loginfo = "page_number(PID:"+QString::number(PID)+" page_id"+QString::number(page_id)+
            " ) :"+QString("%1").arg(page_number, 22-page_size_of_power, 2, QLatin1Char('0'));
    log(loginfo.toLatin1().data());

}

void MainWindow::on_physic_address_button_clicked()
{
    int PID = ui->process_id->text().toInt();
    int page_id = ui->page_id->text().toInt();
    int address = ui->address->text().toInt();

    int physic_address = pt->get_physical_address(PID,page_id,address);


    QString loginfo1 = QString("physic address(PID:%1 page_id:%2 address:%3):").arg(PID).arg(page_id).arg(address);
    QString loginfo2 = QString("%1").arg(physic_address,32,2,QLatin1Char('0'));

    log((loginfo1+loginfo2).toLatin1().data());
}
