#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    ui->label_question_mark->setToolTip("This colour will be applied to the point cloud to be loaded first.");
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

int SettingsDialog::set_default_value(osg::Vec4f &_bgc, osg::Vec4f &_pcc, int _pcs)
{
    ui->btn_bgc->setStyleSheet(rgb2str(_bgc));
    m_bgc = _bgc;

    ui->btn_pcc->setStyleSheet(rgb2str(_pcc));
    m_pcc = _pcc;

    ui->sbx_pcc->setValue(_pcs);
    m_pcs = _pcs;

    return 0;
}

bool SettingsDialog::if_save_as_config()
{
    return ui->cbx_save_as_config->isChecked();
}

bool SettingsDialog::if_background_apply_now()
{
    return ui->cbx_apply_now->isChecked();
}

QString SettingsDialog::rgb2str(osg::Vec4f &c)
{
    QString r,g,b;
    r.setNum(c[0]);
    g.setNum(c[1]);
    b.setNum(c[2]);
    return "background: rgb(" +r+ "," +g+ "," +b+")";
}

void SettingsDialog::on_btn_bgc_clicked()
{
    QColor color = QColorDialog::getColor(QColor(int(m_bgc[0]), int(m_bgc[1]), int(m_bgc[2])), this);
    if (color.isValid())
    {
        float r, g, b;
        r = color.red();
        g = color.green();
        b = color.blue();
        osg::Vec4f tmp(r,g,b,1);
        ui->btn_bgc->setStyleSheet(rgb2str(tmp));
        m_bgc = tmp;
    }
    else {
        //qDebug() << "error";
    }
}

void SettingsDialog::on_btn_pcc_clicked()
{
    QColor color = QColorDialog::getColor(QColor(int(m_pcc[0]), int(m_pcc[1]), int(m_pcc[2])), this);
    if (color.isValid())
    {
        float r, g, b;
        r = color.red();
        g = color.green();
        b = color.blue();
        //qDebug() <<"SettingsDialog"<< r <<" "<< g <<" "<< b;
        osg::Vec4f tmp(r,g,b,1);
        m_pcc = tmp;
        //qDebug() <<"SettingsDialog"<< m_pcc[0] <<" "<< m_pcc[1] <<" "<< m_pcc[2];
        ui->btn_pcc->setStyleSheet(rgb2str(tmp));
    }
    else {
        //qDebug() << "error";
    }
}

void SettingsDialog::on_pushButton_clicked()
{
    m_pcs =  m_pcs = ui->sbx_pcc->text().toInt();
    this->accept();
}

void SettingsDialog::on_pushButton_2_clicked()
{
    this->reject();
}

