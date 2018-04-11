// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef MACRO_H
#define MACRO_H

#include <QObject>

#include "3rd-party/scintilla/qt/ScintillaEdit/ScintillaEdit.h"

#include <QPointer>

class Macro : public QObject {
  Q_OBJECT

  public:
    explicit Macro(const QString& saved_form, QObject* parent = nullptr);
    explicit Macro(QObject* parent = nullptr);

    QString name() const;
    void setName(const QString& name);

    QString toString() const;

  public slots:
    void clear();
    void play(ScintillaEdit* editor);
    void startRecording(ScintillaEdit* editor);
    void endRecording();

  private slots:
    void recordStep(int msg, uptr_t wParam, sptr_t lParam);

  private:
    void fromString(const QString& str);

    struct MacroStep {
      unsigned int m_msg;
      unsigned long m_wParam;
      QByteArray m_text;
    };

    QList<MacroStep> m_macroSteps = QList<MacroStep>();
    QString m_name;

    QPointer<ScintillaEdit> m_attachedEditor = nullptr;
};

#endif // MACRO_H
