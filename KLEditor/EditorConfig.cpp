// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "EditorConfig.h"

using namespace FabricUI::KLEditor;

EditorConfig::EditorConfig()
{
  codeBackgroundColor = QColor(39, 40, 34);
  codeFontSize = 12;
  // codeFont = QFont("DejaVu Sans Mono", codeFontSize);
  codeFont = QFont("Courier New", codeFontSize);
  codeFontColor = QColor(248, 248, 242);
  codeTabWidth = 2;

  lineNumberFontSize = codeFontSize;
  lineNumberFont = codeFont;
  lineNumberBackgroundColor = QColor(39, 40, 34);
  lineNumberFontColor = QColor(139, 140, 135);

  codeCompletionFontSize = codeFontSize;
  codeCompletionFont = codeFont;
  codeCompletionBackgroundColor = QColor(207, 222, 242, 180);
  codeCompletionActiveBackgroundColor = QColor(143, 173, 215, 180);
  codeCompletionFontColor = QColor(0, 0, 0, 180);
  codeCompletionFontColorSuffix = QColor(80, 80, 80, 180);
  codeCompletionMargins = 1;
  codeCompletionSpacing = 0;

  formatForComment.setForeground(QColor( 117, 113, 94));
  formatForString.setForeground(Qt::yellow);
  formatForNumber.setForeground(QColor(147, 112, 219));
  formatForKeyword.setForeground( QColor(255, 0, 127) );
  formatForType.setForeground(Qt::cyan);
  formatForConstant.setForeground(Qt::green);
  formatForMethod.setForeground(Qt::cyan);
  // formatForError.setBackground(QColor(255, 0, 0, 80));
  formatForError.setFontUnderline(true);
  formatForError.setUnderlineColor(QColor(255, 102, 0, 255));
  formatForError.setUnderlineStyle(QTextCharFormat::WaveUnderline);

  formatForHighlight.setBackground(QColor(230, 219, 116, 50));
  formatForHighlight.setProperty(QTextFormat::FullWidthSelection, true);

  editorAutoRebuildAST = true;
  editorAlwaysShowCodeCompletion = true;
  editorSpacing = 2;

  docUrlPrefix = "http://documentation.fabricengine.com/FabricEngine/latest/HTML/";
}
