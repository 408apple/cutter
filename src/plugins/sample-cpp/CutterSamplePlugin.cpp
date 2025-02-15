#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QAction>

#include "CutterSamplePlugin.h"

#include <common/TempConfig.h>
#include <common/Configuration.h>
#include <MainWindow.h>
#include <rz_core.h>

void CutterSamplePlugin::setupPlugin() {}

void CutterSamplePlugin::setupInterface(MainWindow *main)
{
    CutterSamplePluginWidget *widget = new CutterSamplePluginWidget(main);
    main->addPluginDockWidget(widget);
}

CutterSamplePluginWidget::CutterSamplePluginWidget(MainWindow *main) : CutterDockWidget(main)
{
    this->setObjectName("CutterSamplePluginWidget");
    this->setWindowTitle("Sample C++ Plugin");
    QWidget *content = new QWidget();
    this->setWidget(content);

    QVBoxLayout *layout = new QVBoxLayout(content);
    content->setLayout(layout);
    text = new QLabel(content);
    text->setFont(Config()->getFont());
    text->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layout->addWidget(text);

    QPushButton *button = new QPushButton(content);
    button->setText("Want a fortune?");
    button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    button->setMaximumHeight(50);
    button->setMaximumWidth(200);
    layout->addWidget(button);
    layout->setAlignment(button, Qt::AlignHCenter);

    connect(Core(), &CutterCore::seekChanged, this, &CutterSamplePluginWidget::on_seekChanged);
    connect(button, &QPushButton::clicked, this, &CutterSamplePluginWidget::on_buttonClicked);
}

void CutterSamplePluginWidget::on_seekChanged(RVA addr)
{
    Q_UNUSED(addr);
    RzCoreLocked core(Core());
    TempConfig tempConfig;
    tempConfig.set("scr.color", 0);
    QString disasm = Core()->disassembleSingleInstruction(Core()->getOffset());
    QString res = fromOwnedCharPtr(rz_core_clippy(core, disasm.toUtf8().constData()));
    text->setText(res);
}

void CutterSamplePluginWidget::on_buttonClicked()
{
    RzCoreLocked core(Core());
    auto fortune = fromOwned(rz_core_fortune_get_random(core));
    if (!fortune) {
        return;
    }
    QString res = fromOwnedCharPtr(rz_core_clippy(core, fortune.get()));
    text->setText(res);
}
