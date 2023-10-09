#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(const QString window_name, int window_model = 0, QWidget *parent) :
    QDialog(parent),
    m_pc_params(),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(windowFlags() &~ Qt::WindowContextHelpButtonHint);
    this->setWindowFlags(windowFlags() &~ Qt::WindowMaximizeButtonHint);

    this->setWindowTitle(window_name);
    auto fontMetrics = this->fontMetrics();
    auto width = fontMetrics.boundingRect(window_name).width();
    this->setFixedSize(width*4, this->rect().height());

    ui->label_question_mark->setToolTip("This colour will be applied to the point cloud to be loaded first.");

    // setting model for one specific point cloud
    if (window_model == 1)
    {
        ui->cbx_apply_now->setEnabled(false);
        ui->cbx_save_as_config->setEnabled(false);
        ui->label_question_mark->setEnabled(false);
    }
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

int SettingsDialog::set_default_value(pointcloud_params & pc_params)
{
    ui->btn_bgc->setStyleSheet(rgb2str(pc_params.background_color));
    //m_bgc = _bgc;

    ui->btn_pcc->setStyleSheet(rgb2str(pc_params.pointcloud_color));
    //m_pcc = _pcc;

    ui->sbx_pcc->setValue(pc_params.pointcloud_size);
    //m_pcs = _pcs;

    m_pc_params = pc_params;

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
    QColor color = QColorDialog::getColor(
                QColor(int(m_pc_params.background_color[0]), int(m_pc_params.background_color[1]), int(m_pc_params.background_color[2])),
            this);
    if (color.isValid())
    {
        float r, g, b;
        r = color.red();
        g = color.green();
        b = color.blue();
        osg::Vec4f tmp(r,g,b,1);
        ui->btn_bgc->setStyleSheet(rgb2str(tmp));
        m_pc_params.background_color = tmp;
    }
    else {
        //qDebug() << "other";
    }
}

void SettingsDialog::on_btn_pcc_clicked()
{
    QColor color = QColorDialog::getColor(QColor(int(m_pc_params.pointcloud_color[0]), int(m_pc_params.pointcloud_color[1]), int(m_pc_params.pointcloud_color[2])), this);
    if (color.isValid())
    {
        float r, g, b;
        r = color.red();
        g = color.green();
        b = color.blue();
        osg::Vec4f tmp(r,g,b,1);
        m_pc_params.pointcloud_color = tmp;
        ui->btn_pcc->setStyleSheet(rgb2str(tmp));
    }
    else {
        //qDebug() << "other";
    }
}

void SettingsDialog::on_pushButton_clicked()
{
    m_pc_params.pointcloud_size = ui->sbx_pcc->text().toInt();
    this->accept();
}

void SettingsDialog::on_pushButton_2_clicked()
{
    this->reject();
}

