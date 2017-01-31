#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QRect>
#include <QPoint>
#include <QDebug>
#include <QRubberBand>
#include <QStack>
#include <QScrollBar>
#include <QInputDialog>
#include <iostream>
#include <QPainter>
#include <QMouseEvent>
#include <QPainter>
#include <QCloseEvent>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();
    void on_actionZoom_In_triggered();
    void on_actionZoom_Out_triggered();
    void on_actionCrop_triggered();
    void on_rotate_clicked();
    void on_actionReset_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    bool check_border(QMouseEvent *event);
    void closeEvent (QCloseEvent *event);
    void on_actionSave_triggered();
    void on_actionZoom_Selected_triggered();
    void paint_image();
    void on_actionClose_triggered();
    void save_state();
    QImage rotate_image();

private:
    Ui::MainWindow *ui;
    QImage image;
    int height, width;
};

#endif // MAINWINDOW_H
