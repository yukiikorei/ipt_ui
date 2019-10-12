#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "init_dialog.h"
#include "ipt.h"
#include "hasher.h"
#include <QMainWindow>
#include <QDebug>

#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void init();
    void reflash_table();
    void reflash_table_by_process(int PID){reflash_table_by_process(running_processes[PID]);}
    void reflash_table_by_process(const Process& p);
    void clean();

private:
    Ui::MainWindow *ui;
    init_dialog *ind;
    PageTable *pt;

    std::vector<Process> running_processes;
    int current_max_PID;

    int men_size_of_power;
    int page_size_of_power;

    void reflash_table_by_entrys_info(const entrys_info& esi);

private slots:

    void on_random_create_button_clicked();

    void log(const char info[]);


    void on_random_add_button_clicked();

    void on_process_show_clicked();

    void on_process_run_clicked();

    void on_page_number_button_clicked();

    void on_physic_address_button_clicked();



};
#endif // MAINWINDOW_H
