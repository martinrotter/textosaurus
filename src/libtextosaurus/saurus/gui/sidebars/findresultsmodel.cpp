// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/gui/sidebars/findresultsmodel.h"

#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QPainter>
#include <QSize>
#include <QTextDocument>

FindResultsModel::FindResultsModel(QObject* parent)
  : QAbstractItemModel(parent), m_rootItem(new FindResultsModelItem(this)) {}

QModelIndex FindResultsModel::index(int row, int column, const QModelIndex& parent) const {
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }

  FindResultsModelItem* parentItem = itemForIndex(parent);
  FindResultsModelItem* childItem = parentItem->child(row);

  if (childItem) {
    return createIndex(row, column, childItem);
  }
  else {
    return QModelIndex();
  }
}

QModelIndex FindResultsModel::parent(const QModelIndex& child) const {
  if (!child.isValid()) {
    return QModelIndex();
  }

  FindResultsModelItem* childItem = itemForIndex(child);
  FindResultsModelItem* parentItem = childItem->parentItem();

  if (parentItem == m_rootItem.data()) {
    return QModelIndex();
  }
  else {
    return createIndex(parentItem->row(), 0, parentItem);
  }
}

int FindResultsModel::rowCount(const QModelIndex& parent) const {
  if (parent.column() > 0) {
    return 0;
  }

  FindResultsModelItem* parentItem = itemForIndex(parent);

  return parentItem->childCount();
}

int FindResultsModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 1;
}

QVariant FindResultsModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }
  else {
    return itemForIndex(index)->data(role);
  }
}

void FindResultsModel::clear() {
  beginRemoveRows(QModelIndex(), 0, rowCount(QModelIndex()));
  m_rootItem->clearChildren();
  endRemoveRows();
}

void FindResultsModel::addResults(TextEditor* editor, const QList<QPair<int, int>> results) {
  clear();

  beginInsertRows(QModelIndex(), 0, 1);
  FindResultsModelItemEditor* item_editor = new FindResultsModelItemEditor(editor, this);

  for (const QPair<int, int> range : results) {
    int line = editor->lineFromPosition(range.first) + 1;
    QString result_text = QString(editor->textRange(range.first, range.second)).toHtmlEscaped();

    if (result_text.isEmpty()) {
      result_text = "&#10650;";
    }

    QString text = QString("%2<b style=\"color: red;\">%1</b>%3").arg(result_text,
                                                                      QString(editor->textRange(qMax(range.first -
                                                                                                     FIND_RESULT_ENCLOSING_CHAR_COUNT,
                                                                                                     0),
                                                                                                range.first)).toHtmlEscaped(),
                                                                      QString(editor->textRange(range.second,
                                                                                                qMin(range.second +
                                                                                                     FIND_RESULT_ENCLOSING_CHAR_COUNT,
                                                                                                     int(editor->length())))).toHtmlEscaped());
    FindResultsModelItemResult* item_result = new FindResultsModelItemResult(text, line, range, item_editor);

    item_editor->appendChild(item_result);
  }

  m_rootItem->appendChild(item_editor);
  endInsertRows();
}

FindResultsModelItem* FindResultsModel::itemForIndex(const QModelIndex& idx) const {
  if (idx.isValid() && idx.model() == this) {
    return static_cast<FindResultsModelItem*>(idx.internalPointer());
  }
  else {
    return m_rootItem.data();
  }
}

HtmlDelegate::HtmlDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

void HtmlDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
  QStyleOptionViewItem optionV4 = option;

  initStyleOption(&optionV4, index);

  QStyle* style = optionV4.widget ? optionV4.widget->style() : QApplication::style();
  QTextDocument doc;

  doc.setHtml(optionV4.text);
  doc.setDocumentMargin(2);
  optionV4.text = QString();

  style->drawControl(QStyle::CE_ItemViewItem, &optionV4, painter);

  QAbstractTextDocumentLayout::PaintContext ctx;

  if (optionV4.state & QStyle::State_Selected) {
    if (optionV4.state & QStyle::State_HasFocus) {
      ctx.palette.setColor(QPalette::Text, optionV4.palette.color(QPalette::Active, QPalette::HighlightedText));
    }
  }

  QRect text_rect = style->subElementRect(QStyle::SE_ItemViewItemText, &optionV4);

  painter->save();
  painter->translate(text_rect.topLeft());
  painter->setClipRect(text_rect.translated(-text_rect.topLeft()));
  doc.documentLayout()->draw(painter, ctx);
  painter->restore();

}

QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
  QStyleOptionViewItem optionV4 = option;

  initStyleOption(&optionV4, index);

  QTextDocument doc;

  doc.setDocumentMargin(2);
  doc.setHtml(optionV4.text);
  doc.setTextWidth(optionV4.rect.width());
  auto siz = QSize(doc.idealWidth(), doc.size().height());

  return siz;
}
