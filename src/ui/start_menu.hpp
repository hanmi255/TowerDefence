#pragma once

#include <ElaWidget.h>
#include <ElaPushButton.h>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QFontDatabase>

class StartMenu : public QWidget
{
    Q_OBJECT

public:
    StartMenu(QWidget *parent = nullptr) : QWidget(parent)
    {
        // // 加载像素字体
        // int fontId = QFontDatabase::addApplicationFont("res/font/ipix.ttf");
        // QString fontFamily = QFontDatabase::applicationFontFamilies(fontId).first();
        // QFont pixelFont(fontFamily, 24);

        // // 设置窗口属性
        // this->setFixedSize(800, 600);
        // this->setStyleSheet("background-color: #1E1E1E");

        // // 标题
        // QLabel *titleLabel = new QLabel("像素塔防", this);
        // titleLabel->setFont(QFont(fontFamily, 48));
        // titleLabel->setStyleSheet("color: #FFD700; border: 2px solid #FFD700");
        // titleLabel->setAlignment(Qt::AlignCenter);

        // // 按钮样式表
        // QString btnStyle =
        //     "QPushButton {"
        //     "background-color: #2F4F4F;"
        //     "color: white;"
        //     "border: 3px solid #FFD700;"
        //     "padding: 10px;"
        //     "}";

        // // 单人模式按钮
        // QPushButton *singlePlayerBtn = new QPushButton("单人模式", this);
        // singlePlayerBtn->setFont(pixelFont);
        // singlePlayerBtn->setStyleSheet(btnStyle);

        // // 双人模式按钮
        // QPushButton *multiplayerBtn = new QPushButton("双人模式", this);
        // multiplayerBtn->setFont(pixelFont);
        // multiplayerBtn->setStyleSheet(btnStyle);

        // // 退出按钮
        // QPushButton *exitBtn = new QPushButton("退出游戏", this);
        // exitBtn->setFont(pixelFont);
        // exitBtn->setStyleSheet(
        //     "QPushButton {"
        //     "background-color: #8B0000;"
        //     "color: white;"
        //     "border: 3px solid #FFD700;"
        //     "padding: 10px;"
        //     "}");

        // // 关卡选择框
        // QComboBox *levelComboBox = new QComboBox(this);
        // levelComboBox->addItem("关卡 1");
        // levelComboBox->addItem("关卡 2");
        // levelComboBox->addItem("关卡 3");
        // levelComboBox->setFont(QFont(fontFamily, 18));
        // levelComboBox->setStyleSheet(
        //     "QComboBox {"
        //     "background-color: #1E1E1E;"
        //     "color: #00FF00;"
        //     "border: 2px solid #00FF00;"
        //     "padding: 5px;"
        //     "}");

        // // 布局
        // QVBoxLayout *layout = new QVBoxLayout(this);
        // layout->addStretch(1);
        // layout->addWidget(titleLabel, 0, Qt::AlignHCenter);
        // layout->addSpacing(50);
        // layout->addWidget(singlePlayerBtn, 0, Qt::AlignHCenter);
        // layout->addWidget(multiplayerBtn, 0, Qt::AlignHCenter);
        // layout->addWidget(exitBtn, 0, Qt::AlignHCenter);
        // layout->addSpacing(30);
        // layout->addWidget(levelComboBox, 0, Qt::AlignHCenter);
        // layout->addStretch(1);

        // // 连接信号槽
        // connect(exitBtn, &QPushButton::clicked, qApp, &QApplication::quit);
    }
};