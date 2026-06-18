#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <QObject>

class SpellChecker : public QObject
{
    Q_OBJECT
public:
    explicit SpellChecker(QObject *parent = nullptr);

signals:
};

#endif // SPELLCHECKER_H
