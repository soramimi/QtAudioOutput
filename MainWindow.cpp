#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QThread>
#include <QTime>
#include <memory>

class MyAudioOutput : public QThread {
protected:
	void run()
	{
		QAudioFormat format;
		format.setByteOrder(QAudioFormat::LittleEndian);
		format.setChannelCount(1);
		format.setCodec("audio/pcm");
		format.setSampleRate(8000);
		format.setSampleSize(16);
		format.setSampleType(QAudioFormat::SignedInt);
		QAudioOutput player(format);
		QIODevice *device = player.start();

		while (1) {
			if (isInterruptionRequested()) break;

			const int X = 10;

			int16_t buf[16 * X];
			for (int i = 0; i < 16 * X; i++) {
				buf[i] = (i & 8) ? -10000 : 10000;
			}

			while (1) {
				int n = player.bytesFree();
				n /= 32 * X;
				if (n < 1) break;
				for (int i = 0; i < n; i++) {
					device->write((char const *)buf, 32 * X);
				}
			}

			msleep(1);
		}

	}
};

struct MainWindow::Private {
	MyAudioOutput audio_out;
};

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, m(new Private)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	m->audio_out.requestInterruption();
	m->audio_out.wait();
	delete m;
	delete ui;
}

void MainWindow::on_pushButton_clicked()
{
	if (!m->audio_out.isRunning()) {
		m->audio_out.start();
	}
}

