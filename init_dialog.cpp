#include "init_dialog.h"
#include "ui_init_dialog.h"

init_dialog::init_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::init_dialog)
{
    ui->setupUi(this);
}

init_dialog::~init_dialog()
{
    delete ui;
}

void init_dialog::on_buttonBox_accepted()
{
}

int init_dialog::get_men_size_of_power() const{return ui->men_size->value();}

int init_dialog::get_page_size_of_power() const{return ui->page_size->value();}

QString init_dialog::get_hash_name() const{return ui->hash->currentText();}

void init_dialog::on_buttonBox_rejected()
{
}
