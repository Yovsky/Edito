#ifndef ENCDETECTOR_H
#define ENCDETECTOR_H

/*
 * Edito - A modern, cross-platform text editor
 * Copyright (C) 2025 Yovsky <Yovsky@proton.me>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QString>
#include <QStringConverter>
#include <QByteArray>

class encdetector
{
public:
    struct encodingResult
    {
        QString encoding;
        QStringConverter::Encoding converterEnc;
        bool hasBOM;
    };

    static encodingResult detectEncoding(const QByteArray &data);
    static QString detectFileEncoding(const QString &filePath);
    static QList<QStringConverter::Encoding> supportedEncodings();
};

#endif // ENCDETECTOR_H
