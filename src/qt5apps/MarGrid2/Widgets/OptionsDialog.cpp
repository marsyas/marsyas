#include "OptionsDialog.h"

OptionsDialog::OptionsDialog(Grid *grid)
{
  grid_ = grid;
  dialog = new QDialog();
  setupUi(dialog);
  dialog->show();
}

void OptionsDialog::setupUi(QDialog *optionsDialog)
{
  //optionsDialog->resize(384, 426);
  buttonBox = new QDialogButtonBox(optionsDialog);
  buttonBox->setGeometry(QRect(30, 380, 341, 32));
  buttonBox->setOrientation(Qt::Horizontal);
  buttonBox->setStandardButtons(QDialogButtonBox::Ok);

  verticalLayoutWidget = new QWidget(optionsDialog);
  verticalLayoutWidget->setGeometry(QRect(10, 20, 351, 341));

  dialog_v_layout = new QVBoxLayout(verticalLayoutWidget);
  dialog_v_layout->setContentsMargins(0, 0, 0, 0);

  init_box = new QGroupBox(verticalLayoutWidget);

  gridLayoutWidget = new QWidget(init_box);
  gridLayoutWidget->setGeometry(QRect(10, 20, 251, 101));

  init_g_layout = new QGridLayout(gridLayoutWidget);
  init_g_layout->setSpacing(3);
  init_g_layout->setContentsMargins(0, 0, 0, 0);

  init_neighbourhood = new QLabel(gridLayoutWidget);
  init_std_factor = new QLabel(gridLayoutWidget);
  init_neighbour_in = new QLineEdit(gridLayoutWidget);
  init_std_in = new QLineEdit(gridLayoutWidget);
  init_alpha = new QLabel(gridLayoutWidget);
  init_alpha_in = new QLineEdit(gridLayoutWidget);
  init_iterations_label = new QLabel(gridLayoutWidget);
  init_iterations_in = new QLineEdit(gridLayoutWidget);

  init_g_layout->addWidget(init_neighbourhood, 1, 0, 1, 1);
  init_g_layout->addWidget(init_std_factor, 2, 0, 1, 1);
  init_g_layout->addWidget(init_neighbour_in, 1, 2, 1, 1);
  init_g_layout->addWidget(init_std_in, 2, 2, 1, 1);
  init_g_layout->addWidget(init_alpha, 0, 0, 1, 1);
  init_g_layout->addWidget(init_alpha_in, 0, 2, 1, 1);
  init_g_layout->addWidget(init_iterations_label, 3, 0, 1, 1);
  init_g_layout->addWidget(init_iterations_in, 3, 2, 1, 1);

  train_box = new QGroupBox(verticalLayoutWidget);
  gridLayoutWidget_2 = new QWidget(train_box);
  gridLayoutWidget_2->setGeometry(QRect(10, 20, 221, 94));

  train_g_layout = new QGridLayout(gridLayoutWidget_2);
  train_g_layout->setSpacing(3);
  train_g_layout->setContentsMargins(0, 0, 0, 0);

  train_std_factor = new QLabel(gridLayoutWidget_2);
  train_neighbourhood = new QLabel(gridLayoutWidget_2);
  train_alpha_in = new QLineEdit(gridLayoutWidget_2);
  train_neighbour_in = new QLineEdit(gridLayoutWidget_2);
  train_std_in = new QLineEdit(gridLayoutWidget_2);
  train_alpha = new QLabel(gridLayoutWidget_2);
  train_iterations_label = new QLabel(gridLayoutWidget_2);
  train_iterations_in = new QLineEdit(gridLayoutWidget_2);

  train_g_layout->addWidget(train_std_factor, 3, 0, 1, 1);
  train_g_layout->addWidget(train_neighbourhood, 2, 0, 1, 1);
  train_g_layout->addWidget(train_alpha_in, 0, 1, 1, 1);
  train_g_layout->addWidget(train_neighbour_in, 2, 1, 1, 1);
  train_g_layout->addWidget(train_std_in, 3, 1, 1, 1);
  train_g_layout->addWidget(train_alpha, 0, 0, 1, 1);
  train_g_layout->addWidget(train_iterations_label, 4, 0, 1, 1);
  train_g_layout->addWidget(train_iterations_in, 4, 1, 1, 1);

  dialog_v_layout->addWidget(init_box);
  dialog_v_layout->addWidget(train_box);



  // setup labels and input masks
  init_box->setTitle("Init Factors");
  init_std_factor->setText("Std Factor (%)");
  init_alpha->setText("Alpha Decay (%)");
  init_neighbourhood->setText("Neighbourhood Decay (%)");
  init_iterations_label->setText("# of Iterations");
  init_alpha_in->setInputMask("99");
  init_neighbour_in->setInputMask("99");
  init_std_in->setInputMask("99");
  init_alpha_in->setInputMask("99");
  init_iterations_in->setInputMask("99999");

  train_box->setTitle("Training Factors");
  train_alpha->setText("Alpha Decay (%)");
  train_std_factor->setText("Std Factor (%)");
  train_neighbourhood->setText("Neighbourhood Decay (%)");
  train_iterations_label->setText("# of Iterations");
  train_alpha_in->setInputMask("99");
  train_neighbour_in->setInputMask("99");
  train_std_in->setInputMask("99");
  train_iterations_label->setText("# of Iterations");
  train_iterations_in->setInputMask("99999");


  init_alpha_in->setText(QString("%1").arg((int)(grid_->get_init_alpha()*100)));
  init_neighbour_in->setText(QString("%1").arg((int)(grid_->get_init_neighbourhood()*100)));
  init_std_in->setText(QString("%1").arg((int)(grid_->get_init_std_factor()*100)));
  init_iterations_in->setText(QString("%1").arg((grid_->get_init_iterations())));

  train_alpha_in->setText(QString("%1").arg((int)(grid_->get_train_alpha()*100)));
  train_neighbour_in->setText(QString("%1").arg((int)(grid_->get_train_neighbourhood()*100)));
  train_std_in->setText(QString("%1").arg((int)(grid_->get_train_std_factor()*100)));
  train_iterations_in->setText(QString("%1").arg((grid_->get_train_iterations())));


  connect(init_alpha_in, SIGNAL(textEdited(QString)), this, SLOT(init_alpha_changed(QString)));
  connect(init_neighbour_in, SIGNAL(textEdited(QString)), this, SLOT(init_neighborhood_changed(QString)));
  connect(init_std_in, SIGNAL(textEdited(QString)), this, SLOT(init_factor_changed(QString)));
  connect(init_iterations_in, SIGNAL(textEdited(QString)), this, SLOT(init_iterations_changed(QString)));
  connect(train_alpha_in, SIGNAL(textEdited(QString)), this, SLOT(train_alpha_changed(QString)));
  connect(train_neighbour_in, SIGNAL(textEdited(QString)), this, SLOT(train_neighborhood_changed(QString)));
  connect(train_std_in, SIGNAL(textEdited(QString)), this, SLOT(train_factor_changed(QString)));
  connect(train_iterations_in, SIGNAL(textEdited(QString)), this, SLOT(train_iterations_changed(QString)));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(yesButtonClicked()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(noButtonClicked()));

}

/***************************************************
*
* SLOTS
*
***************************************************/

void OptionsDialog::init_alpha_changed(QString input)
{
  grid_->set_init_alpha(input.toDouble() / 100.0);
}
void OptionsDialog::init_neighborhood_changed(QString input)
{
  grid_->set_init_neighbourhood(input.toDouble() / 100.0);
}
void OptionsDialog::init_factor_changed(QString input)
{
  grid_->set_init_std_factor(input.toDouble() / 100.0);
}
void OptionsDialog::init_iterations_changed(QString input)
{
  grid_->set_init_iterations(input.toInt());
}
void OptionsDialog::train_alpha_changed(QString input)
{
  grid_->set_train_alpha(input.toDouble() / 100.0);
}
void OptionsDialog::train_neighborhood_changed(QString input)
{
  grid_->set_train_neighbourhood(input.toDouble() / 100.0);
}
void OptionsDialog::train_factor_changed(QString input)
{
  grid_->set_train_std_factor(input.toDouble() / 100.0);
}
void OptionsDialog::train_iterations_changed(QString input)
{
  grid_->set_train_iterations(input.toInt());
}
void OptionsDialog::yesButtonClicked()
{
  dialog->close();
}
void OptionsDialog::noButtonClicked()
{
  dialog->close();
}
