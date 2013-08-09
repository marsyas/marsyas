#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QStackedWidget>

class PreferencesPage;

class PreferencesDialog : public QDialog
{
  Q_OBJECT

public:
  PreferencesDialog(QWidget *parent=0);
  void addPage(QWidget *page);
  void removePage(QWidget *page);

public slots:
  void changePage(QListWidgetItem *current, QListWidgetItem *previous);

private:
  void createIcons();

  QListWidget *_contentsWidget;
  QStackedWidget *_pagesWidget;
};

#endif /* PREFERENCESDIALOG_H */

