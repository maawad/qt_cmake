// QT
#include <qapplication.h>
#include <qpushbutton.h>
#include <qregularexpression.h>
#include <qtextedit.h>
#include <QFileDialog>
#include <QHBoxLayout>
#include <qstylefactory.h>

// CUDA
#include <cuda.h>
#include <cuda_runtime.h>

// C++
#include <iostream>

// includes
#include <ptx_editor.hpp>

#define cuda_try(call)                                                                \
  do {                                                                                \
    cudaError_t err = static_cast<cudaError_t>(call);                                 \
    if (err != cudaSuccess) {                                                         \
      printf("CUDA error at %s %d: %s\n", __FILE__, __LINE__, cudaGetErrorName(err)); \
      std::terminate();                                                               \
    }                                                                                 \
  } while (0)

CUdevice get_cuda_device(const int device_id, bool quiet = false) {
  CUdevice device;
  int device_count = 0;

  cuda_try(cuInit(0));  // Flag parameter must be zero
  cuda_try(cuDeviceGetCount(&device_count));

  if (device_count == 0) {
    std::cout << "No CUDA capable device found." << std::endl;
    std::terminate();
  }

  cuda_try(cuDeviceGet(&device, device_id));

  cudaDeviceProp device_prop;
  cudaGetDeviceProperties(&device_prop, device_id);

  if (!quiet) { std::cout << "Device[" << device_id << "]: " << device_prop.name << std::endl; }

  return device;
}

int main(int argc, char **argv) {
  QApplication test(argc, argv);
  test.setStyle(QStyleFactory::create("Fusion"));

  QPalette newPalette;
  newPalette.setColor(QPalette::Window, QColor(37, 37, 37));
  newPalette.setColor(QPalette::WindowText, QColor(212, 212, 212));
  newPalette.setColor(QPalette::Base, QColor(60, 60, 60));
  newPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 45));
  newPalette.setColor(QPalette::PlaceholderText, QColor(127, 127, 127));
  newPalette.setColor(QPalette::Text, QColor(212, 212, 212));
  newPalette.setColor(QPalette::Button, QColor(45, 45, 45));
  newPalette.setColor(QPalette::ButtonText, QColor(212, 212, 212));
  newPalette.setColor(QPalette::BrightText, QColor(240, 240, 240));
  newPalette.setColor(QPalette::Highlight, QColor(38, 79, 120));
  newPalette.setColor(QPalette::HighlightedText, QColor(240, 240, 240));

  newPalette.setColor(QPalette::Light, QColor(60, 60, 60));
  newPalette.setColor(QPalette::Midlight, QColor(52, 52, 52));
  newPalette.setColor(QPalette::Dark, QColor(30, 30, 30));
  newPalette.setColor(QPalette::Mid, QColor(37, 37, 37));
  newPalette.setColor(QPalette::Shadow, QColor(0, 0, 0));

  newPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
  test.setPalette(newPalette);



  QWidget *window = new QWidget;
  window->resize(1024, 1024);

  QVBoxLayout *layout = new QVBoxLayout(window);

  QTextEdit *editor            = new QTextEdit;
  QPushButton *load_button     = new QPushButton("Load PTX");
  ptx_highlighter *highlighter = new ptx_highlighter(editor->document());

  QObject::connect(load_button, &QPushButton::clicked, [&]() {
    QString ptx_fname =
        QFileDialog::getOpenFileName(0, "Select source file", ".", "PTX files (*.ptx)");
    QFile ptx_file(ptx_fname);
    if (!ptx_file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&ptx_file);
    editor->clear();
    while (!in.atEnd()) {
      QString line = in.readLine();
      editor->append(line);
    }
  });

  layout->addWidget(editor);
  layout->addWidget(load_button);

  const int device_id = 0;

  CUdevice gpu = get_cuda_device(device_id);
  cudaDeviceProp device_prop;
  cudaGetDeviceProperties(&device_prop, device_id);
  auto gpu_name = device_prop.name;
  editor->setText(gpu_name);

  window->show();
  return test.exec();
}