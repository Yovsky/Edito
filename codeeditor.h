#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QWidget>
#include <QPaintEvent>
#include <QResizeEvent>

class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    void editorActions(QAction *cut, QAction *copy, QAction *paste, QAction *selectAll, QAction *upper, QAction *lower, QAction *webSearch);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void setZoomLevel(int level);
    int getZoomLevel();


protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QAction *a_cut, *a_copy, *a_paste, *a_selectAll;
    QAction *a_upper, *a_lower;
    QAction *a_searchOnWeb;

    QWidget *lineNumberArea;
    int CurrentZoomLevel = 0;
    void onSelectionChanged();

signals:
    void zoomInRequested();
    void zoomOutRequested();
    void cutRequested();
    void copyRequested();
    void pasteRequested();
    void selectAllRequested();
    void selectionStateChanged(bool isSelected);
};

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor) {}

    QSize sizeHint() const override { return QSize(codeEditor->lineNumberAreaWidth(), 0); }

protected:
    void paintEvent(QPaintEvent *event) override { codeEditor->lineNumberAreaPaintEvent(event); }

private:
    CodeEditor *codeEditor;
};

#endif // CODEEDITOR_H
