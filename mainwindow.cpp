#include "mainwindow.h"
#include "ui_mainwindow.h"

QImage first_image;
float scale;
QList<QImage> undo_list;
QList<QImage> redo_list;
QList<int> undo_angle;
QList<int> redo_angle;
QList<float> undo_scale;
QList<float> redo_scale;
QList<int> redo_x;
QList<int> undo_x;
QList<int> redo_y;
QList<int> undo_y;
int xx;
int yy;
int tmp_angle;
QRubberBand *selected_area;
QPoint top_left;
QPoint point;
QPoint gloable_point;
bool selected = false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->slider->setMaximum(360);
    connect( ui->slider, SIGNAL(valueChanged(int)),
             ui->lcd, SLOT(display(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::check_border(QMouseEvent *event){
    return  ui->label->pixmap()->rect().contains(ui->label->mapFromGlobal(event->globalPos()));
}

void MainWindow::save_state()
{
    undo_list.append(image);
    redo_list.clear();
    undo_angle.append(tmp_angle);
    redo_angle.clear();
    undo_scale.append(scale);
    redo_scale.clear();
    redo_x.clear();
    undo_x.append(xx);
    redo_y.clear();
    undo_y.append(yy);
}

void MainWindow::paint_image(){
    int width =  image.width()*scale;
    int height = image.height()*scale;
    QPixmap pixmap(QPixmap::fromImage(image.scaled(width , height, Qt::IgnoreAspectRatio)));
    QMatrix rm;
    rm.rotate(tmp_angle);
    pixmap = pixmap.transformed(rm);
    ui->label->resize(pixmap.size());
    ui->label->setPixmap(pixmap);
    ui->scrollArea->horizontalScrollBar()->setRange(0,ui->label->width());
    ui->scrollArea->horizontalScrollBar()->setValue(xx);
    ui->scrollArea->verticalScrollBar()->setRange(0,ui->label->height());
    ui->scrollArea->verticalScrollBar()->setValue(yy);
}

void remove_selected_area()
{
    if(selected_area){
        selected_area->setVisible(false);
        selected_area = NULL;
        selected = false;
     }
}

void MainWindow::on_actionOpen_triggered()
{
    QFileDialog dialog(this);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open..."), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if (!fileName.isEmpty())
    {
        if(!this->image.isNull()){
            save_state();
        }

        QImage image(fileName);
        first_image = image;
        this->image = image;
        tmp_angle = 0;
        scale = 1;
        xx = 0;
        yy = 0;
        paint_image();
        remove_selected_area();
   }
}

void MainWindow::on_actionZoom_In_triggered()
{
    if(scale > 3.81475){
        return;
    }

    remove_selected_area();
    save_state();
    scale *= 1.25;
    paint_image();
}

void MainWindow::on_actionZoom_Out_triggered()
{

    if(scale < 0.32768){
       return;
    }

    remove_selected_area();
    save_state();
    scale *= 0.8;
    paint_image();
}

void MainWindow::on_rotate_clicked()
{
    remove_selected_area();
    int angle = ui->slider->value();
    save_state();
    tmp_angle += angle;
    paint_image();
}

void MainWindow::on_actionReset_triggered()
{
    remove_selected_area();
    save_state();
    image = first_image;
    tmp_angle = 0;
    scale = 1;
    paint_image();
}

void MainWindow::on_actionUndo_triggered()
{
    //qDebug() << scale << endl;
    if(!undo_list.size())
        return;

    remove_selected_area();
    redo_list.push_back(image);
    image = undo_list.back();
    undo_list.pop_back();
    redo_angle.push_back(tmp_angle);
    tmp_angle = undo_angle.back();
    undo_angle.pop_back();
    redo_scale.push_back(scale);
    scale = undo_scale.back();
    undo_scale.pop_back();
    redo_x.push_back(xx);
    xx = undo_x.back();
    undo_x.pop_back();
    redo_y.push_back(yy);
    yy = undo_y.back();
    undo_y.pop_back();
    paint_image();
}


void MainWindow::on_actionRedo_triggered()
{
    if(!redo_list.size())
        return;

    remove_selected_area();
    undo_list.push_back(image);
    image = redo_list.back();
    redo_list.pop_back();

    undo_angle.push_back(tmp_angle);
    tmp_angle = redo_angle.back();
    redo_angle.pop_back();


    undo_scale.push_back(scale);
    scale = redo_scale.back();
    redo_scale.pop_back();


    undo_x.push_back(xx);
    xx = redo_x.back();
    redo_x.pop_back();

    undo_y.push_back(yy);
    yy = redo_y.back();
    redo_y.pop_back();

    paint_image();
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(!ui->label->pixmap())
        return ;

    if (check_border(event)) {
        point = event->pos();
        gloable_point = event->globalPos();
        selected = true;
    }

    if(selected_area){
        selected_area->setVisible(false);
        selected_area = NULL;
     }

    if(ui->scrollArea->verticalScrollBar()){
        ui->scrollArea->verticalScrollBar()->setEnabled(true);
    }
    if(ui->scrollArea->horizontalScrollBar()){
        ui->scrollArea->horizontalScrollBar()->setEnabled(true);
    }


}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(ui->label->pixmap() && check_border(event) && selected )
    {
        top_left = ui->label->mapFromGlobal(QPoint(std::min(gloable_point.x(),event->globalPos().x()),std::min(gloable_point.y(),event->globalPos().y())));
        height = std::abs(gloable_point.y()-event->globalPos().y());
        width = std::abs(gloable_point.x()-event->globalPos().x());

        if(!selected_area)
        {
            selected_area = new QRubberBand(QRubberBand::Rectangle, this);
        }

        selected_area->show();
        //Returns a normalized rectangle; i.e., a rectangle that has a non-negative width and height.
        selected_area->setGeometry(QRect(point, event->pos()).normalized());
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if( !selected_area )
        return;

    selected = false;

    if(ui->scrollArea->verticalScrollBar()){
        ui->scrollArea->verticalScrollBar()->setEnabled(false);
    }
    if(ui->scrollArea->horizontalScrollBar()){
        ui->scrollArea->horizontalScrollBar()->setEnabled(false);
    }

}

QImage MainWindow::rotate_image(){
    int width =  image.width()*scale;
    int height = image.height()*scale;
    QPixmap pixmap(QPixmap::fromImage(image.scaled(width , height, Qt::IgnoreAspectRatio)));
    QMatrix rm;
    rm.rotate(tmp_angle);
    pixmap = pixmap.transformed(rm);
    return pixmap.toImage();
}



void MainWindow::on_actionCrop_triggered()
{
    if(image.isNull() || !selected_area)
        return;


    save_state();
    QImage temp = rotate_image();
    QPixmap pixmap(QPixmap::fromImage(temp));
    pixmap = pixmap.copy(top_left.x(),top_left.y(),this->width,this->height);
    image = pixmap.toImage();
    tmp_angle = 0;
    scale = 1;
    xx = 0;
    yy = 0;
    paint_image();
    remove_selected_area();

    if(ui->scrollArea->verticalScrollBar()){
        ui->scrollArea->verticalScrollBar()->setEnabled(true);
    }
    if(ui->scrollArea->horizontalScrollBar()){
        ui->scrollArea->horizontalScrollBar()->setEnabled(true);
    }

}



void MainWindow::on_actionSave_triggered()
{
    if(!ui->label->pixmap())
        return ;

    QString filename = QFileDialog::getSaveFileName(this, tr("Save As.."),"",tr("Joint Photographic Experts Group(*.jpg);;Bitmap image(*.bmp);;Portable Network Graphics(.png)") );
    if( !filename.isNull() ){
        QPixmap pixmap(*ui->label->pixmap());
        //The quality factor must be in the range [0,100] or -1.
        //Specify 0 to obtain small compressed files, 100 for large uncompressed files, and -1 to use the default settings.
        pixmap.save(filename,Q_NULLPTR,-1);
    }
}

void MainWindow::on_actionZoom_Selected_triggered()
{
    if(image.isNull() || !selected_area || scale > 3.81475)
        return;

    save_state();
    scale *=  std::min(ui->scrollArea->width()/width,ui->scrollArea->height()/height);

    QPoint tmp = top_left * std::min(ui->scrollArea->width()/width,ui->scrollArea->height()/height);
    xx = tmp.x();
    yy = tmp.y();
    paint_image();
    remove_selected_area();

    if(ui->scrollArea->verticalScrollBar()){
        ui->scrollArea->verticalScrollBar()->setEnabled(true);
    }

    if(ui->scrollArea->horizontalScrollBar()){
        ui->scrollArea->horizontalScrollBar()->setEnabled(true);
    }

}

void MainWindow::on_actionClose_triggered()
{
    QMessageBox::StandardButton prompt = QMessageBox::question(this, "Image Viewer", "Do you want to save?",
                                                                  QMessageBox::Cancel|QMessageBox::No|QMessageBox::Yes);
    if (prompt == QMessageBox::Yes) {
         if(!image.isNull()){
             on_actionSave_triggered();
          }
          exit(0);
    }else if(prompt == QMessageBox::No){
          exit(0);
    }
}

void MainWindow::closeEvent (QCloseEvent *event)
{
   on_actionClose_triggered();
}
