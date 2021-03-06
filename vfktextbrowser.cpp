#include <QDebug>
#include <QTime>
#include <QSqlRecord>
#include <QApplication>
#include <QFile>

#include "vfktextbrowser.h"
#include "htmldocument.h"
#include "latexdocument.h"
#include "richtextdocument.h"


typedef QList<QPair<QByteArray, QByteArray> > TaskList;

VfkTextBrowser::VfkTextBrowser(QWidget *parent) :
  QTextBrowser ( parent ),
  mDocumentBuilder( new DocumentBuilder() )
{
  connect( this, SIGNAL( anchorClicked( QUrl ) ), this, SLOT( onLinkClicked ( QUrl )) );
  connect( this, SIGNAL( updateHistory( HistoryRecord ) ), this, SLOT( saveHistory ( HistoryRecord )) );

  emit currentParIdsChanged( false );
  emit currentBudIdsChanged( false );
  emit historyBefore( false );
  emit historyAfter( false );
}
void VfkTextBrowser::startPage()
{
  processAction( QUrl( "showText?page=allTEL" ) );
}

bool VfkTextBrowser::exportDocument( const QUrl task, const QString fileName, ExportFormat format )
{
  QFile fileOut( fileName );

  if ( !fileOut.open( QIODevice::WriteOnly | QIODevice::Text ) )
  {
    return false;
  }

  TaskMap taskMap = parseTask( task );
  QString text = documentContent( taskMap, format );

  QTextStream streamFileOut( &fileOut );
  streamFileOut.setCodec("UTF-8");
  streamFileOut << text;
  streamFileOut.flush();

  fileOut.close();

  return true;
}

void VfkTextBrowser::setConnectionName(const QString &connectionName)
{
  if ( mDocumentBuilder )
  {
    delete mDocumentBuilder;
  }
  mDocumentBuilder = new DocumentBuilder( connectionName );
}

TaskMap VfkTextBrowser::parseTask( const QUrl task )
{
  TaskList taskList = task.encodedQueryItems();

  TaskMap taskMap;
  taskMap["action"] = task.path();

  TaskList::iterator i;
  for (i = taskList.begin(); i != taskList.end(); ++i)
  {
    taskMap[i->first] = QUrl::fromPercentEncoding(i->second);
  }
  return taskMap;
}

VfkTextBrowser::~VfkTextBrowser()
{
  if ( mDocumentBuilder )
  {
    delete mDocumentBuilder;
  }
}

void VfkTextBrowser::goBack()
{
//  backward();
  if ( mHistoryIt != mUrlHistory.begin() )
  {
    mCurrentRecord = *(--mHistoryIt);
    setHtml( mCurrentRecord.html );
    updateButtonEnabledState();
  }

}


void VfkTextBrowser::goForth()
{
//  forward();
  if ( mHistoryIt != --(mUrlHistory.end()) )
  {
    mCurrentRecord = *(++mHistoryIt);
    setHtml( mCurrentRecord.html );
    updateButtonEnabledState();
  }



}

void VfkTextBrowser::saveHistory( HistoryRecord record )
{
  if ( mUrlHistory.isEmpty() )
  {
    mUrlHistory.append( record );
    mHistoryIt = mUrlHistory.begin();
  }
  else if ( mHistoryIt == --mUrlHistory.end() )
  {
    mUrlHistory.append( record );
    mHistoryIt = --(mUrlHistory.end());
  }
  else
  {
    mUrlHistory.erase( ++mHistoryIt, mUrlHistory.end() );
    mUrlHistory.append( record );
    mHistoryIt = --(mUrlHistory.end());
  }
  mCurrentRecord = *mHistoryIt;
  updateButtonEnabledState();
}

void VfkTextBrowser::showHelpPage()
{
  QString url = QString( "showText?page=help" );
  processAction( QUrl( url ) );
}

void VfkTextBrowser::showInfoAboutSelection( QStringList parIds, QStringList budIds )
{
  QString url;
  // only one is selected
  if ( ( parIds.size() + budIds.size() == 1 ) )
  {
    if ( parIds.size() == 1)
    {
      url = QString( "showText?page=par&id=%1" ).arg( parIds.at( 0 ) );
    }
    else
    {
      url = QString( "showText?page=bud&id=%1" ).arg( budIds.at( 0 ) );
    }
    processAction( QUrl( url ) );
    return;
  }

  QString urlPart;
  if ( !parIds.isEmpty() )
  {
    urlPart += QString( "&parcely=%1" ).arg( parIds.join( "," ) );
  }
  if ( !budIds.isEmpty() )
  {
    urlPart += QString( "&budovy=%1" ).arg( budIds.join( "," ) );
  }
  if ( !urlPart.isEmpty() )
  {
    url = QString( "showText?page=seznam&type=id%1" ).arg( urlPart );
    processAction( QUrl( url ) );
  }

}

void VfkTextBrowser::postInit()
{
  emit currentParIdsChanged( false );
  emit currentBudIdsChanged( false );
  emit historyBefore( false );
  emit historyAfter( false );
  emit definitionPointAvailable( false );
}

VfkDocument *VfkTextBrowser::documentFactory( VfkTextBrowser::ExportFormat format )
{
  VfkDocument *doc;
  switch ( format )
  {
  case VfkTextBrowser::Latex:
    doc = new LatexDocument();
    return doc;
  case VfkTextBrowser::Html:
    doc = new HtmlDocument();
    return doc;
  case VfkTextBrowser::RichText:
    doc = new RichTextDocument();
    return doc;
  default:
    return 0;
  }

}

void VfkTextBrowser::updateButtonEnabledState()
{
  emit currentParIdsChanged( !mCurrentRecord.parIds.isEmpty() );
  emit currentBudIdsChanged( !mCurrentRecord.budIds.isEmpty() );
  emit historyBefore( mHistoryIt != mUrlHistory.begin() );
  emit historyAfter( mHistoryIt != --(mUrlHistory.end()) );
  emit definitionPointAvailable( !( mCurrentRecord.definitionPoint.first.isEmpty() &&
                                    mCurrentRecord.definitionPoint.second.isEmpty() ) );
}

void VfkTextBrowser::onLinkClicked( const QUrl task )
{
  processAction( task );
}

void VfkTextBrowser::processAction( const QUrl task )
{
  mCurrentUrl = task;
  TaskMap taskMap = parseTask( task );

  if ( taskMap["action"] == "showText" )
  {
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
    QTime t;t.start();
    QString html = documentContent( taskMap, VfkTextBrowser::RichText );
    qDebug("Total time elapsed: %d ms", t.elapsed());
    QApplication::restoreOverrideCursor();
    setHtml(html);

    HistoryRecord record = { html, mDocumentBuilder->currentParIds(),
                             mDocumentBuilder->currentBudIds(),
                             mDocumentBuilder->currentDefinitionPoint() };

    emit updateHistory( record );

  }
  else if ( taskMap["action"] == "selectInMap" )
  {
    emit showParcely( taskMap[ "ids" ].split( "," ) );
  }
  else if ( taskMap[ "action" ] == "switchPanel" )
  {
    if ( taskMap[ "panel" ] == "import" )
    {
      emit switchToPanelImport();
    }
    else if( taskMap[ "panel" ] == "search" )
    {
      emit switchToPanelSearch( taskMap[ "type" ].toInt() );
    }
    setHtml( mCurrentRecord.html );
  }
  else
  {
    qDebug() << "jina akce";
  }
}

QString VfkTextBrowser::documentContent( const TaskMap taskMap, VfkTextBrowser::ExportFormat format )
{
  VfkDocument *doc = documentFactory( format );
  if ( !doc )
  {
    return "";
  }
  mDocumentBuilder->buildHtml( doc, taskMap );
  QString text = doc->toString();

  delete doc;

  return text;
}


