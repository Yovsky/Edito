#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QWidget>
#include <QPlainTextEdit>

class LineNumberArea : public QWidget
{
    Q_OBJECT
public:
    explicit LineNumberArea(QPlainTextEdit *editor);

    QSize sizeHint() const override
    {
        return QSize(lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int lineNumberAreaWidth() const;

    QPlainTextEdit *codeEditor;
};

#endif
