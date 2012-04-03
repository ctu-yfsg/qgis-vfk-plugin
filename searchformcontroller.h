#ifndef SEARCHFORMCONTROLLER_H
#define SEARCHFORMCONTROLLER_H

#include "vfktablemodel.h"

#include <QObject>
#include <QUrl>

QT_BEGIN_NAMESPACE

class QComboBox;
class QStackedWidget;
class QPushButton;

class QStandardItemModel;

QT_END_NAMESPACE

class VlastniciSearchForm;
class ParcelySearchForm;
class BudovySearchForm;
class JednotkySearchForm;

class SearchFormController : public QObject
{
  Q_OBJECT

public:
  struct SearchForms
  {
    VlastniciSearchForm *vlastnici;
    ParcelySearchForm *parcely;
    BudovySearchForm *budovy;
    JednotkySearchForm *jednotky;

  };

  struct MainControls
  {
    QComboBox *formComboBox;
    QStackedWidget *searchForms;
    QPushButton *searchButton;
  };

  enum Form { Vlastnici, Parcely, Budovy, Jednotky };

  SearchFormController( const MainControls &mainControls, const SearchForms &searchForms, QString connectionName, QObject *parent );

signals:
  void actionTriggered( QUrl url );

public slots:
  void search();

private slots:
  void changeForm(int index);

private:
  MainControls controls;
  SearchForms forms;
  QString mConnectionName;
  VfkTableModel *mDruhParcely;
  VfkTableModel *mDruhPozemkoveParcely;
  VfkTableModel *mDruhStavebniParcely;
  VfkTableModel *mZpusobVyuzitiBudovy;
  VfkTableModel *mZpusobVyuzitiJednotek;

  void searchVlastnici();
  void searchParcely();
  void searchBudovy();
  void searchJednotky();

  void initForms();
  void initComboBoxModels();

  QStandardItemModel *addFirstRowToModel(QAbstractItemModel *oldModel, QList<QString> newRow);
};

#endif // SEARCHFORMCONTROLLER_H
