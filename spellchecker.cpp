#include "spellchecker.h"

SpellChecker::SpellChecker(QObject *parent)
    : QObject{parent}
    , m_spell(new Hunspell(PROJECT_ROOT "dictionaries/en_US.aff", PROJECT_ROOT "dictionaries/en_US.dic"))
{

}
void SpellChecker::Check(QString content)
{
    QList<QString> wordList = GetList(content);
    for (QString word : wordList)
    {
        if (m_spell->spell(word.toStdString()) != 0)
            qDebug() << word + " is correct";
        else
            qDebug() << word + " has a mistake";
    }
}

QList<QString> SpellChecker::GetList(QString content)
{
    return content.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
}
