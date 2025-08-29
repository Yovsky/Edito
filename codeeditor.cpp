#include "codeeditor.h"
#include "editor.h"
#include <QPainter>
#include <QTextBlock>
#include <QColor>
#include <QMenu>

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , CurrentZoomLevel(0)
{
    setLineWrapMode(NoWrap);

    QFont f("Consolas"); //Setting the font parameters.
    f.setStyleHint(QFont::Monospace);
    setFont(f);

    const int spaceWi = fontMetrics().horizontalAdvance(' '); //Adding the tab spacing.
    setTabStopDistance(spaceWi * 4);

    lineNumberArea = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);
    connect(this, &CodeEditor::selectionChanged, this, &CodeEditor::highlightCurrentLine);
    connect(this, &CodeEditor::selectionChanged, this, &CodeEditor::onSelectionChanged);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

void CodeEditor::editorActions(QAction *cut, QAction *copy, QAction *paste, QAction *selectAll, QAction *upper, QAction *lower, QAction *webSearch)
{
    a_cut = cut;
    a_copy = copy;
    a_paste = paste;
    a_selectAll = selectAll;

    a_upper = upper;
    a_lower = lower;

    a_searchOnWeb = webSearch;
}

void CodeEditor::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);

    menu->addAction(a_cut);
    menu->addAction(a_copy);
    menu->addAction(a_paste);
    menu->addAction(a_selectAll);
    menu->addSeparator();

    menu->addAction(a_upper);
    menu->addAction(a_lower);
    menu->addSeparator();

    menu->addAction(a_searchOnWeb);
    menu->addSeparator();

    menu->exec(event->globalPos());
    delete menu;
}

void CodeEditor::onSelectionChanged()
{
    bool hasSelection = textCursor().hasSelection();
    emit selectionStateChanged(hasSelection);
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 4; //Set the initial margin width.
    int max = qMax(1, blockCount());
    while (max >= 10000) { //Adjust the width for bigger line counts.
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}
void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::red).lighter(15);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(Qt::black).lighter(40)); //Draw the margin (set the color).

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    QTextBlock startBlock = document()->findBlock(textCursor().selectionStart());
    QTextBlock endBlock = document()->findBlock(textCursor().selectionEnd());
    int startLine = startBlock.blockNumber();
    int endLine = endBlock.blockNumber();
    bool hasSelection = textCursor().hasSelection();

    if (startLine > endLine)
        std::swap(startLine, endLine);

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            bool isSelected = hasSelection && (blockNumber >= startLine && blockNumber <= endLine);

            if (isSelected)
                painter.setPen(Qt::green); //Set the numbers parameters.
            else
            {
                int currentLine = textCursor().blockNumber();
                if (blockNumber == currentLine)
                    painter.setPen(Qt::green);
                else
                    painter.setPen(Qt::gray); //Set the numbers parameters.
            }
            painter.drawText(2, top, lineNumberArea->width() - 7, fontMetrics().height(),
                             Qt::AlignRight, number); //Draw the numbers.
        }

        block = block.next(); //The margin numbers logic.
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }

    painter.setPen(QPen (Qt::red, 2)); //Set the line parameters.
    painter.drawLine(lineNumberArea->width()-1, event->rect().top(), lineNumberArea->width()-1, event->rect().bottom()); //Draw the red margin line.
}

void CodeEditor::setZoomLevel(int level)
{
    QFont currentFont = font();
    int baseSize = 10;
    int newSize = baseSize + level;
    currentFont.setPointSize(newSize);
    setFont(currentFont);
    CurrentZoomLevel = level;
}

int CodeEditor::getZoomLevel()
{
    return CurrentZoomLevel;
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Cut))
    {
        emit cutRequested();
        event->accept();
        return;
    }
    if (event->matches(QKeySequence::Copy))
    {
        emit copyRequested();
        event->accept();
        return;
    }
    if (event->matches(QKeySequence::Paste))
    {
        emit pasteRequested();
        event->accept();
        return;
    }
    if (event->matches(QKeySequence::SelectAll))
    {
        emit selectAllRequested();
        event->accept();
        return;
    }
    QPlainTextEdit::keyPressEvent(event);
}
