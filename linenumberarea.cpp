#include "linenumberarea.h"
#include <QPainter>
#include <QTextBlock>

LineNumberArea::LineNumberArea(QPlainTextEdit *editor) : QWidget(editor)
{
    codeEditor = editor;
}

int LineNumberArea::lineNumberAreaWidth() const
{
    int digits = 1;
    int max = qMax(1, codeEditor->blockCount());
    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }

    int space = 3 + codeEditor->fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = codeEditor->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int)codeEditor->blockBoundingGeometry(block)
                  .translated(codeEditor->contentOffset())
                  .top();
    int bottom = top + (int)codeEditor->blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, width() - 2, codeEditor->fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int)codeEditor->blockBoundingRect(block).height();
        ++blockNumber;
    }
}
