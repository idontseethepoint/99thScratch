#pragma once

#include <QWidget>

#include <vector>

class QToolButton;
class QVBoxLayout;
class QFrame;

class CollapsibleSection : public QWidget
{
    Q_OBJECT
public:
    explicit CollapsibleSection(QString const& title, 
        QWidget* parent = nullptr);
    void setContentLayout(QLayout* layout);
    QVBoxLayout* contentLayout() const;
    static CollapsibleSection* Wrap(QString const& title, 
        QWidget* content, QWidget* parent);
    static CollapsibleSection* Wrap(QString const& title, 
        std::vector<QWidget*> contents, QWidget* parent);
private:
    QToolButton* toggleButton_ = nullptr;
    QFrame* headerLine_ = nullptr;
    QWidget* contentWidget_ = nullptr;
    QVBoxLayout* contentLayout_ = nullptr;
};