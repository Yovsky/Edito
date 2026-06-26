#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include "codeeditor.h"
#include <QObject>
#include <third-party/hunspell/hunspell.hxx>
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextCharFormat>

struct WordInfo
{
    QString word;
    qint64 start;
    qint64 end;
};

class SpellChecker : public QObject
{
    Q_OBJECT
public:
    explicit SpellChecker(QObject *parent = nullptr);
    void Check(CodeEditor *editor, QString content);
    QList<WordInfo> GetList(QString content);
    void Suggest(CodeEditor editor, WordInfo words);

private:
    Hunspell *m_spell = nullptr;
    QTextCharFormat m_errorSpellFormat;

signals:
};

#endif // SPELLCHECKER_H
