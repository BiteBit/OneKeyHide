#include "frameless_dialog.h"

#include <QResizeEvent>

FramelessDialog::FramelessDialog(QWidget* parent, const QString& title, int shadowWidth, int titleSubWidth, int titleHeight)
    : SupportFrameless<QDialog>(parent, shadowWidth),
    m_titleSubWidth(titleSubWidth),
    m_titleHeight(titleHeight)
{
    ui.setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);
    ui.labelTitle->setText(title);
    connect(ui.pushButtonClose, SIGNAL(clicked()), this, SLOT(pushButtonClose_clicked()));
}

FramelessDialog::~FramelessDialog() {
}

void FramelessDialog::HideCloseBtn() {
    ui.pushButtonClose->hide();
}

void FramelessDialog::SetTitle(const QString& title) {
	ui.labelTitle->setText(title);
}

void FramelessDialog::pushButtonClose_clicked() {
    DialogClose();
}

void FramelessDialog::resizeEvent(QResizeEvent* event) {
    // ��֪Ϊ��ʹ��frameTitleʹ�ñ���ɫ����������ʹ�ñ���ͼ��Ե����С��һ�����أ��ڴ˵���
    ui.frameTitle->setFixedSize(event->size().width() - 2 * shadow_width_ + 2, m_titleHeight - shadow_width_);
    ui.frameTitle->move(shadow_width_ - 1, shadow_width_ - 1);
}

void FramelessDialog::DialogClose() {
    this->close();
}

//��Ӧwindows��Ϣ
bool FramelessDialog::nativeEvent(const QByteArray& eventType, void* msg, long* result)
{
#ifdef _WIN32
    MSG* message = (MSG*)msg;
    switch(message->message)
    {
    case WM_NCHITTEST:
        int xPos = GET_X_LPARAM(message->lParam) - this->frameGeometry().x();
        int yPos = GET_Y_LPARAM(message->lParam) - this->frameGeometry().y();
        //�ϱ߱���������ť���ұ߿հף��ĸ���
        if(QRect(0, 0, this->width() - m_titleSubWidth , m_titleHeight).contains(xPos,yPos))       //������ �������رհ�ť���ϱ�
            *result = HTCAPTION;
    }

    return SupportFrameless<QDialog>::nativeEvent(eventType, msg, result);
#endif
}
