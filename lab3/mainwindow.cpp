#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "canvas_qt.hpp"
#include "factory_qt.hpp"
#include "error_new.hpp"
#include "error_loader.hpp"
#include <fstream>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), facade_viewer(new Facade), index_model(0), index_camera(0)
{
    ui->setupUi(this);
    scene_view = std::make_shared<QGraphicsScene>();
    director.set_scene(scene_view);
    ui->graphicsView->setScene(scene_view.get());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::render()
{

    if (ui->comboBoxCamera->count() == 0)
    {
        QMessageBox::information(nullptr, "Warning", "Choose camera to render");
        return;
    }

    time_t t_time = time(NULL);

    std::shared_ptr<AbstractFactory> factory;
    auto drawer = director.get_drawer("C:/msys64/home/alena/lab3s/config.cfg", factory);

    if (drawer == nullptr)
    {
        throw OpenStreamError(__FILE__, typeid(*this).name(), __LINE__, ctime(&t_time));
    }

    std::shared_ptr<BaseCommand> command(new DrawCommand(drawer));
    facade_viewer->RunCommand(command);

    ui->graphicsView->setScene(this->scene_view.get());
}

void MainWindow::on_pushButton_AddCamera_clicked()
{
    std::string cam_name = std::string("camera_") + std::to_string(++index_camera);

    try
    {
        std::shared_ptr<BaseCommand> command(new AddCameraCommand(cam_name));
        facade_viewer->RunCommand(command);
        ui->comboBoxCamera->addItem(cam_name.c_str());

        if (ui->comboBoxCamera->count() == 1)
        {
            std::shared_ptr<BaseCommand> command(new SetCameraCommand(cam_name));
            facade_viewer->RunCommand(command);
            if (ui->comboBoxModel->count() > 0)
            {
                render();
            }
        }
    }
    catch (DefaultException &ex)
    {
        QMessageBox::warning(this, "Error message", QString(ex.what()));
    }
}

void MainWindow::on_pushButton_AddModel_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,
                                                QString::fromUtf8("Открыть файл"),
                                                QDir::currentPath(),
                                                "text files (*.txt)");
    if (file.isEmpty())
    {
        return;
    }

    std::string model_name = std::string("model_") + std::to_string(++index_model);
    std::string file_name = file.toLocal8Bit().constData();
    std::cout << 1001;

    try
    {
        std::shared_ptr<BaseCommand> command(new AddModelCommand(model_name, file_name));
        facade_viewer->RunCommand(command);
        std::cout << 1001;
        render();
        std::cout << 1001;
        ui->comboBoxModel->addItem(model_name.c_str());
    }
    catch (DefaultException &ex)
    {
        QMessageBox::warning(this, "Error message", QString(ex.what()));
    }
}

void MainWindow::on_pushButton_SetCamera_clicked()
{
    std::string cam_name = ui->comboBoxCamera->currentText().toStdString();
    try
    {
        std::shared_ptr<BaseCommand> command(new SetCameraCommand(cam_name));
        facade_viewer->RunCommand(command);

        if (ui->comboBoxCamera->count() > 0)
        {
            render();
        }
    }
    catch (DefaultException &ex)
    {
        QMessageBox::warning(this, "Error message", QString(ex.what()));
    }
}

void MainWindow::on_pushButton_moveModel_clicked()
{
    if (ui->comboBoxModel->count() == 0)
    {
        return;
    }

    std::string obj_name = ui->comboBoxModel->currentText().toStdString();
    double x = ui->doubleSpinBoxDx->value();
    double y = ui->doubleSpinBoxDy->value();
    double z = ui->doubleSpinBoxDz->value();

    try
    {
        Point<double> moving(x, y, z);
        Point<double> rotateing(0, 0, 0);
        Point<double> scale(1, 1, 1);
        std::shared_ptr<BaseCommand> command(new TransformModelCommand(obj_name, moving, scale, rotateing));
        facade_viewer->RunCommand(command);
    }
    catch (DefaultException &ex)
    {
        QMessageBox::warning(this, "Error message", QString(ex.what()));
    }

    render();
}

void MainWindow::on_pushButton_scaleModel_clicked()
{
    if (ui->comboBoxModel->count() == 0)
    {
        return;
    }

    std::string obj_name = ui->comboBoxModel->currentText().toStdString();
    double x = ui->doubleSpinBoxKx->value();
    double y = ui->doubleSpinBoxKy->value();
    double z = ui->doubleSpinBoxKz->value();

    try
    {
        Point<double> moving(0, 0, 0);
        Point<double> rotateing(0, 0, 0);
        Point<double> scale(x, y, z);
        std::shared_ptr<BaseCommand> command(new TransformModelCommand(obj_name, moving, scale, rotateing));
        facade_viewer->RunCommand(command);
    }
    catch (DefaultException &ex)
    {
        QMessageBox::warning(this, "Error message", QString(ex.what()));
    }

    render();
}

void MainWindow::on_pushButton_rotateModel_clicked()
{
    if (ui->comboBoxModel->count() == 0)
    {
        return;
    }

    std::string obj_name = ui->comboBoxModel->currentText().toStdString();
    double x = ui->doubleSpinBoxOx->value();
    double y = ui->doubleSpinBoxOy->value();
    double z = ui->doubleSpinBoxOz->value();

    try
    {
        Point<double> moving(0, 0, 0);
        Point<double> rotateing(x, y, z);
        Point<double> scale(1, 1, 1);
        std::shared_ptr<BaseCommand> command(new TransformModelCommand(obj_name, moving, scale, rotateing));
        facade_viewer->RunCommand(command);
    }
    catch (DefaultException &ex)
    {
        QMessageBox::warning(this, "Error message", QString(ex.what()));
    }

    render();
}

std::string MainWindow::readConfigFile(const char *filename)
{
    std::ifstream file_;
    time_t t_time = time(NULL);

    file_.open(filename);
    if (!file_)
    {
        throw OpenStreamError(__FILE__, typeid(*this).name(), __LINE__, ctime(&t_time));
    }

    std::string dev_file;
    std::getline(file_, dev_file);
    file_.close();

    return dev_file;
}

void MainWindow::on_pushButton_moveCamera_clicked()
{
    if (ui->comboBoxModel->count() == 0)
    {
        return;
    }

    std::string cam_name = ui->comboBoxCamera->currentText().toStdString();
    double x = ui->doubleSpinBoxDx_c->value();
    double y = ui->doubleSpinBoxDy_c->value();
    double z = ui->doubleSpinBoxDz_c->value();

    try
    {
        Point<double> moving(x, y, z);
        Point<double> rotateing(0, 0, 0);
        std::shared_ptr<BaseCommand> command(new TransformCameraCommand(cam_name, moving, rotateing));
        facade_viewer->RunCommand(command);
    }
    catch (DefaultException &ex)
    {
        QMessageBox::warning(this, "Error message", QString(ex.what()));
    }

    render();
}

void MainWindow::on_pushButton_rotateCamera_clicked()
{
    if (ui->comboBoxModel->count() == 0)
    {
        return;
    }

    std::string cam_name = ui->comboBoxCamera->currentText().toStdString();
    double angle_x = ui->doubleSpinBoxOx_c->value();
    double angle_y = ui->doubleSpinBoxOy_c->value();
    double angle_z = ui->doubleSpinBoxOz_c->value();

    try
    {
        Point<double> moving(0, 0, 0);
        Point<double> rotateing(angle_x, angle_y, angle_z);
        std::shared_ptr<BaseCommand> command(new TransformCameraCommand(cam_name, moving, rotateing));
        facade_viewer->RunCommand(command);
    }
    catch (DefaultException &ex)
    {
        QMessageBox::warning(this, "Error message", QString(ex.what()));
    }

    render();
}

void MainWindow::on_pushButton_RemoveCamera_clicked()
{
    if (ui->comboBoxCamera->count() == 0)
    {
        return;
    }
    try
    {
        std::string cam_name = ui->comboBoxCamera->currentText().toStdString();
        std::shared_ptr<BaseCommand> command(new RemoveCameraCommand(cam_name));
        facade_viewer->RunCommand(command);
        ui->comboBoxCamera->removeItem(ui->comboBoxCamera->currentIndex());
        if (ui->comboBoxCamera->count() == 0)
        {
            QMessageBox::information(nullptr, "Warning", "Choose camera to render");
            scene_view->clear();

            return;
        }

        render();
    }
    catch (DefaultException &ex)
    {
        QMessageBox::warning(this, "Error message", QString(ex.what()));
    }
}

void MainWindow::on_pushButton_RemoveModel_clicked()
{
    if (ui->comboBoxCamera->count() == 0)
    {
        return;
    }

    try
    {
        std::string model_name = ui->comboBoxModel->currentText().toStdString();
        std::shared_ptr<BaseCommand> command(new RemoveModelCommand(model_name));
        facade_viewer->RunCommand(command);
        ui->comboBoxModel->removeItem(ui->comboBoxModel->currentIndex());
        if (ui->comboBoxCamera->count() == 0)
        {
            QMessageBox::information(nullptr, "Warning", "Choose camera to render");
            scene_view->clear();

            return;
        }
        if (ui->comboBoxModel->count() == 0)
        {
            scene_view->clear();

            return;
        }

        render();
    }
    catch (DefaultException &ex)
    {
        QMessageBox::warning(this, "Error message", QString(ex.what()));
    }
}

std::shared_ptr<BaseDrawer> MainWindow::DrawDirector::get_drawer(const char *fp, std::shared_ptr<AbstractFactory> &f)
{
    MainWindow obj;
    if (obj.readConfigFile(fp) == "Qt")
    {
        this->_scene_view->clear();
        f = std::make_shared<QtFactory>();
        auto drawer = f->CreateUi();
        drawer->setCanvas(std::make_shared<QtCanvas>(this->_scene_view));

        return drawer;
    }

    return nullptr;
}
