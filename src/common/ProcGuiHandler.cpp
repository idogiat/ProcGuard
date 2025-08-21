#include "ProcGuiHandler.hpp"
#include "ProcStatusMgr.hpp"

#include <QHeaderView>
#include <QTimer>
#include <vector>


ProcGuiHandler::ProcGuiHandler(QWidget* parent) : QWidget(parent)
{
    table = new QTableWidget(this);
    table->setColumnCount(3);
    QStringList headers = {"PID", "Status", "Extra Info"};
    table->setHorizontalHeaderLabels(headers);
    table->horizontalHeader()->setStretchLastSection(true);

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
    for (auto pid : pids) {
        table->setItem(row, 0, new QTableWidgetItem(QString::number(pid.pid)));
        table->setItem(row, 1, new QTableWidgetItem(ProcStatusToString(pid.status)));
        table->setItem(row, 2, new QTableWidgetItem(QString::number(pid.active)));
        row++;
    }
}
