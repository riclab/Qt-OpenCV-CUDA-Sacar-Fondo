#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * Metodo para obtener la direccion del video
 * @brief MainWindow::SeleccionarVideo
 */
void MainWindow::SeleccionarVideo(){
    // Declara la variable con la ruta del archivo
    QString archivo = QFileDialog::getOpenFileName(this, tr("Abrir Video"),
                                                    "",
                                                    tr("Videos (*.avi *.mp4 *.mov)"));
    //Agrega la ruta del archivo
    ui->labelVideo->setText(archivo);
    ui->radioVideo->setChecked(true);
}

/**
 * Metodo para procesar el video frame a frame si ckecked==true
 * @brief MainWindow::ProcesarVideo
 * @param checked
 *
 */
void MainWindow::ProcesarVideo(bool checked){

    cv::destroyAllWindows(); // Para cerrar todas las ventanas

    cv::Ptr< cv::cuda::BackgroundSubtractorMOG2 > MOG2_cuda = cv::cuda::createBackgroundSubtractorMOG2();
    cv::Mat Mascara_Mog;
    cv::cuda::GpuMat Mascara_Mog_cuda;

    cv::Mat frame; // Frame como array multidimencional

    cv::cuda::GpuMat frame_cuda;




    if(!checked){ // Si !checked detiene el video si no lo procesa
        ui->play->setText("Iniciar video");
        cap.release();
    }else{
        ui->play->setText("Parar video");

        if(ui->radioVideo->isChecked()){ // si el "radio button" esta seleccionado ejecuta el video si no la webcam
            cap.open(ui->labelVideo->text().toStdString().c_str());
        }else{
            cap.open(0);
        }

    }

    while(checked) // bucle hasta que se precione "parar video"
    {
        cap >> frame; // obtiene un nuevo frame del video o camara
        if(frame.empty()) break; // detiene el bucle si elframe esta vacio

        frame_cuda.upload(frame);
        MOG2_cuda->apply(frame_cuda, Mascara_Mog_cuda, -1);
        Mascara_Mog_cuda.download(Mascara_Mog);


        cv::namedWindow("Reproductor", cv::WINDOW_KEEPRATIO); // creamos una ventana la cual permita redimencionar
        cv::imshow("Reproductor", Mascara_Mog); // se muestran los frames
        char key = (char) cv::waitKey(20); //espera 20ms por la tecla ESC
        if(key == 27) break; //detiene el bucle
    }

}

void MainWindow::on_toolButton_clicked()
{
    SeleccionarVideo();
}

void MainWindow::on_actionAbrir_Video_triggered()
{
    SeleccionarVideo();
}



void MainWindow::on_play_toggled(bool checked)
{
    ProcesarVideo(checked);
}
