#include "spellchecker.h"

SpellChecker::SpellChecker(QObject *parent)
    : QObject{parent}
    , m_spell(new Hunspell(PROJECT_ROOT "dictionaries/en_US.aff", PROJECT_ROOT "dictionaries/en_US.dic"))
{

}
void SpellChecker::Check(QString content)
{
    QList<WordInfo> wordList = GetList(content);
    for (WordInfo w : wordList)
    {
        if (m_spell->spell(w.word.toStdString()) == 0)
        {
            qDebug() << "Word " + w.word + " from " + QString::number(w.start) + " to " + QString::number(w.end) + " has a mistake";
        }
    }
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
