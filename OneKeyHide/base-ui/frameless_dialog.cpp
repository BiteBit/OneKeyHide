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
    // 不知为何使用frameTitle使用背景色可以正常，使用背景图边缘总是小了一个像素，在此调整
    ui.frameTitle->setFixedSize(event->size().width() - 2 * shadow_width_ + 2, m_titleHeight - shadow_width_);
    ui.frameTitle->move(shadow_width_ - 1, shadow_width_ - 1);
}

void FramelessDialog::DialogClose() {
    this->close();
}

//响应windows消息
bool FramelessDialog::nativeEvent(const QByteArray& eventType, void* msg, long* result)
{
#ifdef _WIN32
    MSG* message = (MSG*)msg;
    switch(message->message)
    {
    case WM_NCHITTEST:
        int xPos = GET_X_LPARAM(message->lParam) - this->frameGeometry().x();
        int yPos = GET_Y_LPARAM(message->lParam) - this->frameGeometry().y();
        //上边标题栏，按钮栏右边空白，四个边
        if(QRect(0, 0, this->width() - m_titleSubWidth , m_titleHeight).contains(xPos,yPos))       //标题栏 不包括关闭按钮，上边
            *result = HTCAPTION;
    }

    return SupportFrameless<QDialog>::nativeEvent(eventType, msg, result);
#endif
}
