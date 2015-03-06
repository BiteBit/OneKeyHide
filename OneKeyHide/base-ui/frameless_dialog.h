#ifndef FRAMELESSDIALOG_DEMO_H_
#define FRAMELESSDIALOG_DEMO_H_

#include <QDialog>

#include "ui_frameless_dialog.h"
#include "supportframess.h"

class FramelessDialog : 
    public SupportFrameless<QDialog>
{
    Q_OBJECT

public:
    FramelessDialog(QWidget* parent, const QString& title = "", int shadowWidget = 5, int titleSubWidth = 30, int titleHeight = 30);
    ~FramelessDialog();

    void HideCloseBtn();

	void SetTitle(const QString& title);
public slots:
    void pushButtonClose_clicked();

protected:
    virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

    virtual void resizeEvent(QResizeEvent* event) override;

    virtual void DialogClose();
private:
    Ui::BaseFrameLessDialog ui;

    int m_titleSubWidth;
    int m_titleHeight;
};

#endif // FRAMELESSDIALOG_DEMO_H_
