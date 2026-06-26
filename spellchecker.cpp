#include "spellchecker.h"

SpellChecker::SpellChecker(QObject *parent)
    : QObject{parent}
    , m_spell(new Hunspell(PROJECT_ROOT "dictionaries/en_US.aff", PROJECT_ROOT "dictionaries/en_US.dic"))
{
    m_errorSpellFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    m_errorSpellFormat.setUnderlineColor(Qt::red);
}

void SpellChecker::Check(CodeEditor* editor, QString content)
{
    if (!editor) return;

    editor->blockSignals(true);

    QTextCursor originalCursor = editor->textCursor();

    QTextCursor clearCursor(editor->document());
    clearCursor.select(QTextCursor::Document);
    QTextCharFormat clearFormat;
    clearFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);
    clearCursor.mergeCharFormat(clearFormat);

    QTextCursor markerCursor(editor->document());
    QList<WordInfo> wordList = GetList(content);

    for (const WordInfo &w : wordList)
    {
        if (m_spell->spell(w.word.toStdString()) == 0)
        {
            markerCursor.setPosition(w.start);
            markerCursor.setPosition(w.end, QTextCursor::KeepAnchor);
            markerCursor.mergeCharFormat(m_errorSpellFormat);
        }
    }

    editor->setTextCursor(originalCursor);
    editor->blockSignals(false);
}

QList<WordInfo> SpellChecker::GetList(QString content)
{
    QRegularExpression regex("\\b\\w+\\b");
    QRegularExpressionMatchIterator iterator = regex.globalMatch(content);

    QList<WordInfo> res;
    while (iterator.hasNext())
    {
        QRegularExpressionMatch match = iterator.next();

        WordInfo w;
        w.word = match.captured();
        w.start = match.capturedStart();
        w.end = match.capturedEnd();
        res.append(w);
    }
    return res;
}

void SpellChecker::Suggest(CodeEditor editor, WordInfo words)
{

}
