#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QObject>
#include <third-party/hunspell/hunspell.hxx>
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>

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
    void Check(QString content);
    QList<WordInfo> GetList(QString content);

private:
    Hunspell *m_spell = nullptr;

signals:
};

#endif // SPELLCHECKER_H
