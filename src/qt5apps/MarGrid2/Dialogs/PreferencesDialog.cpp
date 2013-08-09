#include "PreferencesDialog.h"

#include <QListWidgetItem>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

PreferencesDialog::PreferencesDialog(QWidget *parent)
  : QDialog(parent)
{
  _contentsWidget = new QListWidget;
  _contentsWidget->setViewMode(QListView::IconMode);
  _contentsWidget->setIconSize(QSize(64, 64));
  _contentsWidget->setMovement(QListView::Static);
  _contentsWidget->setMaximumWidth(128);
  _contentsWidget->setSpacing(12);

  _pagesWidget = new QStackedWidget;

  QPushButton *closeButton = new QPushButton(tr("Close"));

  createIcons();
  _contentsWidget->setCurrentRow(0);

  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

  QHBoxLayout *horzLayout = new QHBoxLayout;
  horzLayout->addWidget(_contentsWidget);
  horzLayout->addWidget(_pagesWidget, 1);

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addStretch(1);
  buttonLayout->addWidget(closeButton);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(horzLayout);
  mainLayout->addStretch(1);
  mainLayout->addSpacing(12);
  mainLayout->addLayout(buttonLayout);
  setLayout(mainLayout);

  setWindowTitle(tr("Preferences"));
}

void PreferencesDialog::addPage(QWidget *page) {
  if ( page && -1 == _pagesWidget->indexOf(page) ) {
    _pagesWidget->addWidget(page);
  }
}

void PreferencesDialog::removePage(QWidget *page) {
  if ( page && -1 != _pagesWidget->indexOf(page) ) {
    _pagesWidget->removeWidget(page);
  }
}

void PreferencesDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous) {
  if ( !current )
    current = previous;
  _pagesWidget->setCurrentIndex(_contentsWidget->row(current));
}

void PreferencesDialog::createIcons() {
  QListWidgetItem *configButton = new QListWidgetItem(_contentsWidget);
  configButton->setIcon(QIcon(":/images/config.png"));
  configButton->setText(tr("Configuration"));
  configButton->setTextAlignment(Qt::AlignHCenter);
  configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

  QListWidgetItem *updateButton = new QListWidgetItem(_contentsWidget);
  updateButton->setIcon(QIcon(":/images/update.png"));
  updateButton->setText(tr("Update"));
  updateButton->setTextAlignment(Qt::AlignHCenter);
  updateButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

  QListWidgetItem *queryButton = new QListWidgetItem(_contentsWidget);
  queryButton->setIcon(QIcon(":/images/query.png"));
  queryButton->setText(tr("Query"));
  queryButton->setTextAlignment(Qt::AlignHCenter);
  queryButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

  connect(_contentsWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
          this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
}

