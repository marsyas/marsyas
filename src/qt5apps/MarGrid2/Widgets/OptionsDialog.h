
#ifndef OPTIONS_DIALOG_H
#define OPTIONS_DIALOG_H

#include "Grid.h"

#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>

class OptionsDialog : public QObject
{
  Q_OBJECT
public:
  OptionsDialog(Grid *grid);

public slots:
  void init_alpha_changed(QString);
  void init_neighborhood_changed(QString);
  void init_factor_changed(QString);
  void init_iterations_changed(QString);

  void train_alpha_changed(QString);
  void train_neighborhood_changed(QString);
  void train_factor_changed(QString);
  void train_iterations_changed(QString);
  void noButtonClicked();
  void yesButtonClicked();

signals:
  void init_alpha_signal(int);
  void init_neighborhood_signal(int);
  void init_factor_signal(int);
  void init_iterations_signal(int);

  void train_alpha_signal(int);
  void train_neighborhood_signal(int);
  void train_factor_signal(int);
  void train_iterations_signal(int);

protected:
  void setupUi(QDialog *optionsDialog);

private:
  QDialog *dialog;
  Grid *grid_;
  QDialogButtonBox *buttonBox;
  QVBoxLayout *dialog_v_layout;

  QGridLayout *init_g_layout;
  QGridLayout *train_g_layout;

  QGroupBox *init_box;
  QGroupBox *train_box;

  QWidget *gridLayoutWidget_2;
  QWidget *gridLayoutWidget;
  QWidget *verticalLayoutWidget;

  QLabel *init_alpha;
  QLabel *init_iterations_label;
  QLabel *init_neighbourhood;
  QLabel *init_std_factor;

  QLabel *train_alpha;
  QLabel *train_std_factor;
  QLabel *train_neighbourhood;
  QLabel *train_iterations_label;

  QLineEdit *init_alpha_in;
  QLineEdit *init_iterations_in;
  QLineEdit *init_neighbour_in;
  QLineEdit *init_std_in;
  QLineEdit *train_iterations_in;
  QLineEdit *train_alpha_in;
  QLineEdit *train_neighbour_in;
  QLineEdit *train_std_in;
};
# endif
