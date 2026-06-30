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
    void Check(CodeEditor *editor);
    QList<WordInfo> GetList(QString content);
    QList<QString> Suggest(CodeEditor *editor, QString word);
    bool IsMisspelled(CodeEditor *editor, qint64 pos);

private:
    Hunspell *m_spell = nullptr;
    QTextCharFormat m_errorSpellFormat;
    QList<WordInfo> m_misspelled;

signals:
};

#endif // SPELLCHECKER_H
