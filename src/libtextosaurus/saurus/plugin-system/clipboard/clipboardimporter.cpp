// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#include "saurus/plugin-system/clipboard/clipboardimporter.h"

#include "common/gui/messagebox.h"
#include "common/miscellaneous/iofactory.h"
#include "saurus/gui/texteditor.h"
#include "saurus/miscellaneous/textapplication.h"
#include "saurus/miscellaneous/textapplicationsettings.h"
#include "saurus/plugin-system/clipboard/clipboardmodel.h"

#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QUrl>

ClipboardImporter::ClipboardImporter(ClipboardItem* entry, TextApplication* text_app, TextEditor* editor, QWidget* parent)
  : QDialog(parent), m_entry(entry), m_textApp(text_app), m_activeEditor(editor) {
  m_ui.setupUi(this);

  addSupportedActions();
  adjustSize();

  connect(this, &ClipboardImporter::accepted, this, &ClipboardImporter::performImport);
}

void ClipboardImporter::performImport() {
  switch (selectedTarget()) {
    case ClipboardImporter::ImportTarget::ImportUrl: {
      QStringList urls;

      for (const QUrl& url : m_entry->data()->urls()) {
        urls << url.toString();
      }

      QString text = urls.join(QL1S(", "));

      if (m_activeEditor->selectionEmpty()) {
        m_activeEditor->insertText(m_activeEditor->currentPos(), text.toUtf8().constData());
        m_activeEditor->gotoPos(m_activeEditor->currentPos() + text.size());
      }
      else {
        m_activeEditor->replaceSel(text.toUtf8().constData());
      }

      m_activeEditor->setFocus();
      break;
    }

    case ClipboardImporter::ImportTarget::ImportHtml: {
      QString text = m_entry->data()->html();

      if (m_activeEditor->selectionEmpty()) {
        m_activeEditor->insertText(m_activeEditor->currentPos(), text.toUtf8().constData());
        m_activeEditor->gotoPos(m_activeEditor->currentPos() + text.size());
      }
      else {
        m_activeEditor->replaceSel(text.toUtf8().constData());
      }

      m_activeEditor->setFocus();
      break;
    }

    case ClipboardImporter::ImportTarget::ImportText: {
      QString text = m_entry->data()->text();

      if (m_activeEditor->selectionEmpty()) {
        m_activeEditor->insertText(m_activeEditor->currentPos(), text.toUtf8().constData());
        m_activeEditor->gotoPos(m_activeEditor->currentPos() + text.size());
      }
      else {
        m_activeEditor->replaceSel(text.toUtf8().constData());
      }

      m_activeEditor->setFocus();
      break;
    }

    case ClipboardImporter::ImportTarget::ImportBase64FileUrlData: {
      QString text;

      for (const QUrl& url : m_entry->data()->urls()) {
        if (url.isLocalFile()) {
          QFileInfo file(url.toLocalFile());

          if (file.exists()) {

            text += file.fileName();
            text += m_textApp->settings()->eolString();
            text += IOFactory::readFile(file.absoluteFilePath()).toBase64();
            text += m_textApp->settings()->eolString();
            text += m_textApp->settings()->eolString();
          }
          else {
            qWarningNN << tr("File '") << file.absoluteFilePath() << tr("' does not exist and cannot be imported as Base64.");
          }
        }
      }

      if (!text.isEmpty()) {
        if (m_activeEditor->selectionEmpty()) {
          m_activeEditor->insertText(m_activeEditor->currentPos(), text.toUtf8().constData());
          m_activeEditor->gotoPos(m_activeEditor->currentPos() + text.size());
        }
        else {
          m_activeEditor->replaceSel(text.toUtf8().constData());
        }

        m_activeEditor->setFocus();
      }

      break;
    }

    case ClipboardImporter::ImportTarget::SaveBase64ClipboardData: {
      if (m_entry->data()->hasUrls() && m_entry->data()->urls().at(0).isLocalFile()) {
        // Save linked files into folder.
        QString folder = MessageBox::getExistingDirectory(parentWidget(),
                                                          tr("Select Output Directory"),
                                                          QDir::homePath());

        if (!folder.isEmpty()) {
          for (const QUrl& url : m_entry->data()->urls()) {
            if (url.isLocalFile()) {
              QString source_file = url.toLocalFile();
              QString target_file = folder + QDir::separator() + QFileInfo(source_file).fileName();

              if (!QFile::copy(source_file, target_file)) {
                qCriticalNN << QSL("Failed to copy file '") << QDir::toNativeSeparators(source_file)
                            << QSL("' to destination '") << QDir::toNativeSeparators(target_file) << QSL("'.");
              }
            }
          }
        }
      }
      else {
        // Save raw clipboard contents to single file.
        QString file = MessageBox::getSaveFileName(parentWidget(),
                                                   tr("Save Clipboard to File"),
                                                   QDir::homePath(),
                                                   QStringList() << tr("All files (*)"),
                                                   nullptr);

        if (!file.isEmpty()) {
          IOFactory::writeFile(file, m_entry->data()->data(m_entry->data()->formats().at(0)));
        }
      }

      break;
    }

    case ClipboardImporter::ImportTarget::ImportBase64ClipboardData:  {
      QByteArray data = m_entry->data()->data(m_entry->data()->formats().at(0));

      if (data.size() > DATA_CLIPBOARD_TOO_BIG_WARNING) {
        if (MessageBox::show(parentWidget(),
                             QMessageBox::Icon::Question,
                             tr("Clipboard Data Too Big"),
                             tr("Clipboard data you want to import in Base64 format is quite big."),
                             tr("Do you want to import data to editor anyway?"),
                             QString(),
                             QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No,
                             QMessageBox::StandardButton::Yes) != QMessageBox::StandardButton::Yes) {
          return;
        }
      }

      QString text = data.toBase64();

      if (m_activeEditor->selectionEmpty()) {
        m_activeEditor->insertText(m_activeEditor->currentPos(), text.toUtf8().constData());
        m_activeEditor->gotoPos(m_activeEditor->currentPos() + text.size());
      }
      else {
        m_activeEditor->replaceSel(text.toUtf8().constData());
      }

      m_activeEditor->setFocus();
      break;
    }
  }
}

ClipboardImporter::ImportTarget ClipboardImporter::selectedTarget() const {
  return ClipboardImporter::ImportTarget(m_ui.m_cmbOptions->currentData().toInt());
}

void ClipboardImporter::addSupportedActions() {
  if (m_entry->data()->hasText()) {
    m_ui.m_cmbOptions->addItem(tr("Import plain text"), int(ImportTarget::ImportText));
  }

  if (m_entry->data()->hasHtml()) {
    m_ui.m_cmbOptions->addItem(tr("Import HTML"), int(ImportTarget::ImportHtml));
  }

  if (m_entry->data()->hasUrls()) {
    m_ui.m_cmbOptions->addItem(tr("Import URL link(s) or file path(s)"), int(ImportTarget::ImportUrl));

    if (m_entry->type() == ClipboardItem::ItemType::File) {
      m_ui.m_cmbOptions->addItem(tr("Import Base64 file(s) data"), int(ImportTarget::ImportBase64FileUrlData));
    }
  }

  m_ui.m_cmbOptions->addItem(tr("Import raw clipboard data as Base64"), int(ImportTarget::ImportBase64ClipboardData));
  m_ui.m_cmbOptions->addItem(tr("Save raw clipboard data (or files) to file (or folder)"), int(ImportTarget::SaveBase64ClipboardData));
}
