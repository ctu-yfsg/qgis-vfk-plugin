#include "htmldocument.h"

#include <QRegExp>

HtmlDocument::HtmlDocument()
{
}

QString HtmlDocument::toString()
{
  QString page = "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">";
  page += mPage;
  page.replace( "&", "&amp;" );
  page.replace( QRegExp( "<a[^>]*>([^<]*)</a>" ), "\\1" );
  return page;
}

void HtmlDocument::header()
{
  mPage +=
      "<html><head>"
      "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">"
      "<meta http-equiv=\"content-language\" content=\"cs\">"
      "<title></title>"
      "<style type=\"text/css\">"
      "body{"
      "  background-color: white;"
      "  color: black;"
      "}"
      "table tr:nth-child(odd) td{"
      //"  background-color: #ffff00;"
      "  background-color: #ffff55;"
      "}"
      "table tr:nth-child(even) td{"
      //"  background-color: #ffc400;"
      "  background-color: #ffff99;"
      "}"
      "table th{"
      //"  background-color: #ffa500;"
      "  background-color: #ffbb22;"
      "}"
      "table{"
      "border: 0;"
      //"  border-spacing: 0;"
      "  border-collapse:collapse;"
      "  border:1px solid #ffbb1d;"
      "  margin:0px 0px;"
      "}"
      "table td, table th{"
      "  padding-left: 0.5em;"
      "  padding-right: 0.5em;"
      "  padding-top: 0.2em;"
      "  padding-bottom: 0.2em;"
      "}"
      "</style>"
      "</head><body>";

  titleIsSet = false;
}

void HtmlDocument::footer()
{
  mPage += "</body></html>";
}

void HtmlDocument::heading1( const QString &text )
{
  mPage += QString( "<h1>%1</h1>" ).arg( text );

  if ( !titleIsSet )
  {
    title( text );
  }
}

void HtmlDocument::heading2( const QString &text )
{
   mPage += QString( "<h2>%1</h2>" ).arg( text );
}

void HtmlDocument::heading3( const QString &text )
{
  mPage += QString( "<h3>%1</h3>" ).arg( text );
}

void HtmlDocument::beginItemize()
{
  mPage += "<ul>";
}

void HtmlDocument::endItemize()
{
  mPage += "</ul>";
}

void HtmlDocument::beginItem()
{
  mPage += "<li>";
}

void HtmlDocument::endItem()
{
  mPage += "</li>";
}

void HtmlDocument::item( const QString &text )
{
  mPage += QString( "<li>%1</li>" ).arg( text );
}

void HtmlDocument::beginTable()
{
  mPage += "<table>";
}

void HtmlDocument::endTable()
{
  mPage += "</table>";
}

void HtmlDocument::tableHeader( const QStringList &columns )
{
  mPage += "<tr>";
  foreach( QString column, columns )
  {
    mPage += QString( "<th>%1</th>" ).arg( column );
  }

  mPage += "</tr>";
  mLastColumnNumber = columns.size();
}

void HtmlDocument::tableRow( const QStringList &columns )
{
  mPage += "<tr>";
  foreach( QString column, columns )
  {
    mPage += QString( "<td>%1</td>" ).arg( column );
  }
  mPage += "</tr>";

  mLastColumnNumber = columns.size();
}

void HtmlDocument::tableRowOneColumnSpan(const QString &text)
{
  mPage += "<tr>";
  mPage += QString( "<td colspan=\"%1\">%2</td>" ).arg( mLastColumnNumber ).arg( text );
  mPage += "</tr>";
}

QString HtmlDocument::link( const QString &href, const QString &text )
{
  return QString( "<a href=\"%1\">%2</a>" ).arg( href ).arg( text );
}

QString HtmlDocument::superscript(const QString &text)
{
  return QString( "<sup><small>%1</small></sup>" ).arg( text );
}

QString HtmlDocument::newLine()
{
  return QString( "<br/>" );
}

void HtmlDocument::keyValueTable( const KeyValList &content )
{
  mPage += "<table>";

  for ( KeyValList::ConstIterator it = content.begin(); it != content.end(); ++it )
  {
    mPage += QString( "<tr>" );
    mPage += QString( "<td>%2</td>" ).arg( it->first );
    mPage += QString( "<td>%2</td>" ).arg( it->second );
    mPage += QString( "</tr>" );
  }
  mPage += QString( "</table>" );
}

void HtmlDocument::paragraph( const QString &text )
{
  mPage += QString( "<p>%1</p>" ).arg( text );
}

void HtmlDocument::table( const TableContent &content, bool header )
{
  beginTable();
  int i = 0;
  if ( header && !content.isEmpty() )
  {
    tableHeader( content.at( 0 ) );
    i++;
  }
  for ( ; i < content.size(); i++ )
  {
    tableRow( content.at( i ) );
  }

  endTable();
}

void HtmlDocument::text( const QString &text )
{
  mPage += text;
}

void HtmlDocument::discardLastBeginTable()
{
  int index = mPage.lastIndexOf( "<table" );
  mPage.chop( mPage.size() - index );
}

bool HtmlDocument::isLastTableEmpty()
{
  return mPage.contains( QRegExp( "<table[^>]*>$" ) );
}

void HtmlDocument::title( const QString &text )
{
  mPage.replace( QRegExp( "<title>.*</title>" ), QString( "<title>%1</title>" ).arg( text ) );
  titleIsSet = true;
}

