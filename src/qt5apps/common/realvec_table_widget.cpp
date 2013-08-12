#include "realvec_table_widget.h"

#include <QtAlgorithms>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>

namespace MarsyasQt {

void RealvecTableWidget::keyPressEvent(QKeyEvent *event)
{
    if (event == QKeySequence::Copy)
    {
        QModelIndexList selection = selectedIndexes();
        qSort(selection);

        QStringList strings;
        foreach ( const QModelIndex & index, selection )
        {
            strings << model()->data(index, Qt::DisplayRole).toString();
        }
        QString text = strings.join(", ");
        QApplication::clipboard()->setText(text);
        return;
    }

    QTableView::keyPressEvent(event);
}

}
