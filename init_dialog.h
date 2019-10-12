#ifndef INIT_DIALOG_H
#define INIT_DIALOG_H

#include <QDialog>

namespace Ui {
class init_dialog;
}

class init_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit init_dialog(QWidget *parent = nullptr);
    ~init_dialog();

    int get_men_size_of_power()const;
    int get_page_size_of_power()const;

    QString get_hash_name() const;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::init_dialog *ui;
};

#endif // INIT_DIALOG_H
