#include "ProcGuiHandler.hpp"
#include "ProcStatusMgr.hpp"
#include "ProcUtils.hpp"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QTimer>
#include <QLabel>
#include <vector>


ProcGuiHandler::ProcGuiHandler(QWidget* parent) : QWidget(parent)
{
    table = new QTableWidget(this);
    QStringList headers = {"PID", "Status", "Name", "Owner", "Sub pids", "Information"};
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setStretchLastSection(true);

    // Layout to make table resize with window
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(table);
    setLayout(layout);

    // Timer to refresh every 2 seconds
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ProcGuiHandler::refreshTable);
    timer->start(2000);

    // Initial fill
    refreshTable();

    setWindowTitle("ProcGuiHandler - Live Process Monitor");
    resize(600, 400);
}

ProcGuiHandler::~ProcGuiHandler() = default;

void ProcGuiHandler::refreshTable()
{
    ProcStatusMgr& shm = ProcStatusMgr::getInstance();
    std::vector<PidEntry> pids = shm.getAllPIDs();
    table->setRowCount(pids.size());

    int row = 0;
    for (auto pid : pids)
    {
        table->setItem(row, 0, new QTableWidgetItem(QString::number(pid.pid)));
        table->setItem(row, 1, new QTableWidgetItem(ProcStatusToString(pid.status)));
        table->setItem(row, 2, new QTableWidgetItem(getProcessName(pid.pid).c_str()));
        table->setItem(row, 3, new QTableWidgetItem(getProcessOwner(pid.pid).c_str()));
        table->setItem(row, 4, new QTableWidgetItem(QString::number(countChildren(pid.pid))));

        QString info = QString::fromStdString(getProcessMoreInfo(pid.pid));
        QLabel *label = new QLabel(info);
        label->setWordWrap(true);
        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        table->setCellWidget(row, 5, label);
        row++;
    }
    table->resizeRowsToContents();
}
