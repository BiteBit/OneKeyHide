#ifndef SUPPORTFRAMESS
#define SUPPORTFRAMESS

#include <QPainter>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <WindowsX.h>
#endif

template <typename T>
class SupportFrameless: public T
{
public:
    SupportFrameless(QWidget* parent, int shadow_width = 5, bool enable_resize = false)
        :typename T(parent),
        shadow_width_(shadow_width),
        enable_resizing_(enable_resize) {
        Qt::WindowFlags flag = windowFlags();
        flag |= Qt::FramelessWindowHint;
        this->setWindowFlags(flag);
    }

protected:
    // 判断是否命中阴影，派生类应自行判断是否命中标题，即设置 HTCAPTION
    virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result) override
    {
        if (!enable_resizing_)
            return *result != 0;

#ifdef _WIN32
        MSG* msg = (MSG*)message;
        switch(msg->message)
        {
        case WM_NCHITTEST:
            {
                int xPos = GET_X_LPARAM(msg->lParam) - this->frameGeometry().x();
                int yPos = GET_Y_LPARAM(msg->lParam) - this->frameGeometry().y();

                if(xPos > shadow_width_ - 2 && xPos < shadow_width_ + 2)
                    *result = HTLEFT;
                else if(xPos > (this->width() - shadow_width_ - 2) && xPos < (this->width()))
                    *result = HTRIGHT;
                else if(yPos > shadow_width_ - 2 && yPos < shadow_width_ + 2)
                    *result = HTTOP;
                else if(yPos > (this->height() - shadow_width_ - 2) && yPos < (this->height() - shadow_width_ + 2))
                    *result = HTBOTTOM;
                else if(xPos > shadow_width_ - 2 && xPos < shadow_width_ + 2 && yPos > 18 && yPos < shadow_width_ +2)
                    *result = HTTOPLEFT;
                else if(xPos > (this->width() - shadow_width_ - 2) && xPos < (this->width() - shadow_width_ + 2) && yPos > shadow_width_ - 2 && yPos < shadow_width_ + 2)
                    *result = HTTOPRIGHT;
                else if(xPos > shadow_width_ - 2 && xPos < shadow_width_ + 2 && yPos > (this->height() - shadow_width_ - 2) && yPos < (this->height() - shadow_width_ - 2))
                    *result = HTBOTTOMLEFT;
                else if(xPos > (this->width() - shadow_width_ - 2) && xPos < (this->width() - shadow_width_ + 2) && yPos > (this->height() - shadow_width_ - 2) && yPos < (this->height() - shadow_width_ + 2))
                    *result = HTBOTTOMRIGHT;
            }
            break;
        }
#endif // ifdef OS_WIN

        return *result != 0;
    }

    virtual void paintEvent(QPaintEvent* event) override
    {
        //绘制程序周围阴影
        QPainter p(this);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(240,237,237));
        p.drawRoundedRect(QRect(shadow_width_,shadow_width_,this->width() - 2*shadow_width_,this->height() - 2*shadow_width_),2.0f,2.0f);

		QList<QPixmap> pixmaps;
		pixmaps.append(QPixmap(":/OneKeyHide/Resources/border_shadow/shadow_left.png"));
		pixmaps.append(QPixmap(":/OneKeyHide/Resources/border_shadow/shadow_right.png"));
		pixmaps.append(QPixmap(":/OneKeyHide/Resources/border_shadow/shadow_top.png"));
		pixmaps.append(QPixmap(":/OneKeyHide/Resources/border_shadow/shadow_bottom.png"));
		pixmaps.append(QPixmap(":/OneKeyHide/Resources/border_shadow/shadow_corner1.png"));
		pixmaps.append(QPixmap(":/OneKeyHide/Resources/border_shadow/shadow_corner2.png"));
		pixmaps.append(QPixmap(":/OneKeyHide/Resources/border_shadow/shadow_corner3.png"));
		pixmaps.append(QPixmap(":/OneKeyHide/Resources/border_shadow/shadow_corner4.png"));
        //四角
        p.drawPixmap(0,0,shadow_width_,shadow_width_,pixmaps[4]);
        p.drawPixmap(this->width() - shadow_width_,0,shadow_width_,shadow_width_,pixmaps[5]);
        p.drawPixmap(0,this->height() - shadow_width_,shadow_width_,shadow_width_,pixmaps[6]);
        p.drawPixmap(this->width() - shadow_width_,this->height() - shadow_width_,shadow_width_,shadow_width_,pixmaps[7]);

        //左
        p.drawPixmap(0,shadow_width_,shadow_width_,this->height() - shadow_width_*2,pixmaps[0].scaled(shadow_width_,this->height() - shadow_width_*2));
        //右
        p.drawPixmap(this->width() - shadow_width_,shadow_width_,shadow_width_,this->height() - shadow_width_*2,pixmaps[1].scaled(shadow_width_,this->height() - shadow_width_*2));
        //上
        p.drawPixmap(shadow_width_,0,this->width() - shadow_width_*2,shadow_width_,pixmaps[2].scaled(this->width() - shadow_width_*2,shadow_width_));
        //下
        p.drawPixmap(shadow_width_,this->height() - shadow_width_,this->width() - shadow_width_*2,shadow_width_,pixmaps[3].scaled(this->width() - shadow_width_*2,shadow_width_));
    }

protected:
    int shadow_width_;
    bool enable_resizing_;
};

#endif // SUPPORTFRAMESS

