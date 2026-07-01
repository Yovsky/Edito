#include "spellchecker.h"

SpellChecker::SpellChecker(QObject *parent)
    : QObject{parent}
    , m_spell(new Hunspell(PROJECT_ROOT "dictionaries/en_US.aff", PROJECT_ROOT "dictionaries/en_US.dic"))
{
    m_errorSpellFormat.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    m_errorSpellFormat.setUnderlineColor(Qt::red);
}

void SpellChecker::Check(CodeEditor* editor)
{
    if (!editor) return;
    QString content = editor->document()->toPlainText();

    QList<QTextEdit::ExtraSelection> selections;
    QList<WordInfo> wordList = GetList(content);

    for (const WordInfo &w : wordList)
    {
        if (m_spell->spell(w.word.toStdString()) == 0)
        {
            // add misspelled word to the misspelled list
            m_misspelled.append(w);

            // underline the misspelled word
            QTextEdit::ExtraSelection selection;
            QTextCursor markerCursor(editor->document());

            markerCursor.setPosition(w.start);
            markerCursor.setPosition(w.end, QTextCursor::KeepAnchor);

            selection.format = m_errorSpellFormat;
            selection.cursor = markerCursor;

            selections.append(selection);
        }
    }

    // apply the underlining
    editor->SetSpellcheckerSelections(selections);
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

QList<QString> SpellChecker::Suggest(CodeEditor* editor, QString word)
{
    QList<QString> corrections;

    // create and populate the suggestions list
    char **list = nullptr;
    int count = m_spell->suggest(&list, word.toStdString().c_str());

    // safety and convert to QString list
    if (count > 0 && list != nullptr)
    {
        for (int i = 0; i < count; i++)
        {
            corrections.append(QString::fromUtf8(list[i]));
        }
    }

    m_spell->free_list(&list, count);

    return corrections;
}

bool SpellChecker::IsMisspelled(CodeEditor* editor, qint64 pos)
{
    Check(editor);
    for (WordInfo w : m_misspelled)
    {
        if (pos >= w.start && pos <= w.end)
            return true;
    }
    return false;
}
