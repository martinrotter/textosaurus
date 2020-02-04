// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef MACRO_H
#define MACRO_H

#include <QObject>

#include "definitions/definitions.h"

// *INDENT-OFF*
#include PATH(SCINTILLA_DIR,qt/ScintillaEdit/ScintillaEdit.h)

// *INDENT-ON*

#include <QPointer>

class Macro : public QObject {
  Q_OBJECT

  public:
    enum class ScintillaCommand {
      //! Move down one line.
      LineDown = SCI_LINEDOWN,

      //! Extend the selection down one line.
      LineDownExtend = SCI_LINEDOWNEXTEND,

      //! Extend the rectangular selection down one line.
      LineDownRectExtend = SCI_LINEDOWNRECTEXTEND,

      //! Scroll the view down one line.
      LineScrollDown = SCI_LINESCROLLDOWN,

      //! Move up one line.
      LineUp = SCI_LINEUP,

      //! Extend the selection up one line.
      LineUpExtend = SCI_LINEUPEXTEND,

      //! Extend the rectangular selection up one line.
      LineUpRectExtend = SCI_LINEUPRECTEXTEND,

      //! Scroll the view up one line.
      LineScrollUp = SCI_LINESCROLLUP,

      //! Scroll to the start of the document.
      ScrollToStart = SCI_SCROLLTOSTART,

      //! Scroll to the end of the document.
      ScrollToEnd = SCI_SCROLLTOEND,

      //! Scroll vertically to centre the current line.
      VerticalCentreCaret = SCI_VERTICALCENTRECARET,

      //! Move down one paragraph.
      ParaDown = SCI_PARADOWN,

      //! Extend the selection down one paragraph.
      ParaDownExtend = SCI_PARADOWNEXTEND,

      //! Move up one paragraph.
      ParaUp = SCI_PARAUP,

      //! Extend the selection up one paragraph.
      ParaUpExtend = SCI_PARAUPEXTEND,

      //! Move left one character.
      CharLeft = SCI_CHARLEFT,

      //! Extend the selection left one character.
      CharLeftExtend = SCI_CHARLEFTEXTEND,

      //! Extend the rectangular selection left one character.
      CharLeftRectExtend = SCI_CHARLEFTRECTEXTEND,

      //! Move right one character.
      CharRight = SCI_CHARRIGHT,

      //! Extend the selection right one character.
      CharRightExtend = SCI_CHARRIGHTEXTEND,

      //! Extend the rectangular selection right one character.
      CharRightRectExtend = SCI_CHARRIGHTRECTEXTEND,

      //! Move left one word.
      WordLeft = SCI_WORDLEFT,

      //! Extend the selection left one word.
      WordLeftExtend = SCI_WORDLEFTEXTEND,

      //! Move right one word.
      WordRight = SCI_WORDRIGHT,

      //! Extend the selection right one word.
      WordRightExtend = SCI_WORDRIGHTEXTEND,

      //! Move to the end of the previous word.
      WordLeftEnd = SCI_WORDLEFTEND,

      //! Extend the selection to the end of the previous word.
      WordLeftEndExtend = SCI_WORDLEFTENDEXTEND,

      //! Move to the end of the next word.
      WordRightEnd = SCI_WORDRIGHTEND,

      //! Extend the selection to the end of the next word.
      WordRightEndExtend = SCI_WORDRIGHTENDEXTEND,

      //! Move left one word part.
      WordPartLeft = SCI_WORDPARTLEFT,

      //! Extend the selection left one word part.
      WordPartLeftExtend = SCI_WORDPARTLEFTEXTEND,

      //! Move right one word part.
      WordPartRight = SCI_WORDPARTRIGHT,

      //! Extend the selection right one word part.
      WordPartRightExtend = SCI_WORDPARTRIGHTEXTEND,

      //! Move to the start of the document line.
      Home = SCI_HOME,

      //! Extend the selection to the start of the document line.
      HomeExtend = SCI_HOMEEXTEND,

      //! Extend the rectangular selection to the start of the document line.
      HomeRectExtend = SCI_HOMERECTEXTEND,

      //! Move to the start of the displayed line.
      HomeDisplay = SCI_HOMEDISPLAY,

      //! Extend the selection to the start of the displayed line.
      HomeDisplayExtend = SCI_HOMEDISPLAYEXTEND,

      //! Move to the start of the displayed or document line.
      HomeWrap = SCI_HOMEWRAP,

      //! Extend the selection to the start of the displayed or document
      //! line.
      HomeWrapExtend = SCI_HOMEWRAPEXTEND,

      //! Move to the first visible character in the document line.
      VCHome = SCI_VCHOME,

      //! Extend the selection to the first visible character in the document
      //! line.
      VCHomeExtend = SCI_VCHOMEEXTEND,

      //! Extend the rectangular selection to the first visible character in
      //! the document line.
      VCHomeRectExtend = SCI_VCHOMERECTEXTEND,

      //! Move to the first visible character of the displayed or document
      //! line.
      VCHomeWrap = SCI_VCHOMEWRAP,

      //! Extend the selection to the first visible character of the
      //! displayed or document line.
      VCHomeWrapExtend = SCI_VCHOMEWRAPEXTEND,

      //! Move to the end of the document line.
      LineEnd = SCI_LINEEND,

      //! Extend the selection to the end of the document line.
      LineEndExtend = SCI_LINEENDEXTEND,

      //! Extend the rectangular selection to the end of the document line.
      LineEndRectExtend = SCI_LINEENDRECTEXTEND,

      //! Move to the end of the displayed line.
      LineEndDisplay = SCI_LINEENDDISPLAY,

      //! Extend the selection to the end of the displayed line.
      LineEndDisplayExtend = SCI_LINEENDDISPLAYEXTEND,

      //! Move to the end of the displayed or document line.
      LineEndWrap = SCI_LINEENDWRAP,

      //! Extend the selection to the end of the displayed or document line.
      LineEndWrapExtend = SCI_LINEENDWRAPEXTEND,

      //! Move to the start of the document.
      DocumentStart = SCI_DOCUMENTSTART,

      //! Extend the selection to the start of the document.
      DocumentStartExtend = SCI_DOCUMENTSTARTEXTEND,

      //! Move to the end of the document.
      DocumentEnd = SCI_DOCUMENTEND,

      //! Extend the selection to the end of the document.
      DocumentEndExtend = SCI_DOCUMENTENDEXTEND,

      //! Move up one page.
      PageUp = SCI_PAGEUP,

      //! Extend the selection up one page.
      PageUpExtend = SCI_PAGEUPEXTEND,

      //! Extend the rectangular selection up one page.
      PageUpRectExtend = SCI_PAGEUPRECTEXTEND,

      //! Move down one page.
      PageDown = SCI_PAGEDOWN,

      //! Extend the selection down one page.
      PageDownExtend = SCI_PAGEDOWNEXTEND,

      //! Extend the rectangular selection down one page.
      PageDownRectExtend = SCI_PAGEDOWNRECTEXTEND,

      //! Stuttered move up one page.
      StutteredPageUp = SCI_STUTTEREDPAGEUP,

      //! Stuttered extend the selection up one page.
      StutteredPageUpExtend = SCI_STUTTEREDPAGEUPEXTEND,

      //! Stuttered move down one page.
      StutteredPageDown = SCI_STUTTEREDPAGEDOWN,

      //! Stuttered extend the selection down one page.
      StutteredPageDownExtend = SCI_STUTTEREDPAGEDOWNEXTEND,

      //! Delete the current character.
      Delete = SCI_CLEAR,

      //! Delete the previous character.
      DeleteBack = SCI_DELETEBACK,

      //! Delete the previous character if not at start of line.
      DeleteBackNotLine = SCI_DELETEBACKNOTLINE,

      //! Delete the word to the left.
      DeleteWordLeft = SCI_DELWORDLEFT,

      //! Delete the word to the right.
      DeleteWordRight = SCI_DELWORDRIGHT,

      //! Delete right to the end of the next word.
      DeleteWordRightEnd = SCI_DELWORDRIGHTEND,

      //! Delete the line to the left.
      DeleteLineLeft = SCI_DELLINELEFT,

      //! Delete the line to the right.
      DeleteLineRight = SCI_DELLINERIGHT,

      //! Delete the current line.
      LineDelete = SCI_LINEDELETE,

      //! Cut the current line to the clipboard.
      LineCut = SCI_LINECUT,

      //! Copy the current line to the clipboard.
      LineCopy = SCI_LINECOPY,

      //! Transpose the current and previous lines.
      LineTranspose = SCI_LINETRANSPOSE,

      //! Duplicate the current line.
      LineDuplicate = SCI_LINEDUPLICATE,

      //! Select the whole document.
      SelectAll = SCI_SELECTALL,

      //! Move the selected lines up one line.
      MoveSelectedLinesUp = SCI_MOVESELECTEDLINESUP,

      //! Move the selected lines down one line.
      MoveSelectedLinesDown = SCI_MOVESELECTEDLINESDOWN,

      //! Duplicate the selection.
      SelectionDuplicate = SCI_SELECTIONDUPLICATE,

      //! Convert the selection to lower case.
      SelectionLowerCase = SCI_LOWERCASE,

      //! Convert the selection to upper case.
      SelectionUpperCase = SCI_UPPERCASE,

      //! Cut the selection to the clipboard.
      SelectionCut = SCI_CUT,

      //! Copy the selection to the clipboard.
      SelectionCopy = SCI_COPY,

      //! Paste from the clipboard.
      Paste = SCI_PASTE,

      //! Toggle insert/overtype.
      EditToggleOvertype = SCI_EDITTOGGLEOVERTYPE,

      //! Insert a platform dependent newline.
      Newline = SCI_NEWLINE,

      //! Insert a formfeed.
      Formfeed = SCI_FORMFEED,

      //! Indent one level.
      Tab = SCI_TAB,

      //! De-indent one level.
      Backtab = SCI_BACKTAB,

      //! Cancel any current operation.
      Cancel = SCI_CANCEL,

      //! Undo the last command.
      Undo = SCI_UNDO,

      //! Redo the last command.
      Redo = SCI_REDO,

      //! Zoom in.
      ZoomIn = SCI_ZOOMIN,

      //! Zoom out.
      ZoomOut = SCI_ZOOMOUT,

      //! Text operation or replaced selection.
      TextOperationReplaceSel = SCI_REPLACESEL,

      //! Text inserted
      TextInserted = SCI_INSERTTEXT
    };

    Q_ENUM(ScintillaCommand)

    struct MacroStep {
      ScintillaCommand m_cmd;
      int m_msg;
      uptr_t m_wParam;
      QByteArray m_text;
    };

    explicit Macro(const QString& saved_form, QObject* parent = nullptr);
    explicit Macro(QObject* parent = nullptr);

    QString name() const;
    void setName(const QString& name);

    QString toString() const;
    QList<MacroStep> macroSteps() const;

  public slots:
    void clear();
    void play(ScintillaEdit* editor);
    void startRecording(ScintillaEdit* editor);
    void endRecording();

  signals:
    void newStepRecorded(MacroStep macro_step);

  private slots:
    void recordStep(int msg, uptr_t wParam, sptr_t lParam);

  private:
    void fromString(const QString& asc);

    QList<MacroStep> m_macroSteps = QList<MacroStep>();
    QString m_name;

    QPointer<ScintillaEdit> m_attachedEditor = nullptr;
};

#endif // MACRO_H
