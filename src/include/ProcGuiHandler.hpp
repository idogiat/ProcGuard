#pragma once
#include <QWidget>
#include <QTableWidget>

class ProcGuiHandler : public QWidget
{
    Q_OBJECT
public:
    explicit ProcGuiHandler(QWidget *parent = nullptr);
    ~ProcGuiHandler();
    void refreshTable();

private:
    QTableWidget* table;
};