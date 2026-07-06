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

#include "encdetector.h"
#include <QFile>
#include <QDebug>
#include <uchardet.h>

encdetector::encodingResult encdetector::detectEncoding(const QByteArray &data)
{
    QMap<QString, QStringConverter::Encoding> encodings;
    encodings["UTF-8"] = QStringConverter::Encoding::Utf8;
    encodings["UTF-16LE"] = QStringConverter::Encoding::Utf16LE;
    encodings["UTF-16BE"] = QStringConverter::Encoding::Utf16BE;
    encodings["UTF-32LE"] = QStringConverter::Encoding::Utf32LE;
    encodings["UTF-32BE"] = QStringConverter::Encoding::Utf32BE;
    encodings["ISO-8859-1"] = QStringConverter::Encoding::Latin1;
    encodings["System"] = QStringConverter::Encoding::System;

    encdetector::encodingResult res;
    res.hasBOM = false;

    qDebug() << "Data size:" << data.size() << "bytes";

    qDebug() << data.left(8).toHex(' ');

    //Try detecting by BOM.
    if(data.startsWith(QByteArray::fromRawData("\xEF\xBB\xBF", 3)))
    {
        qDebug() << "detected UTF-8";
        res.encoding = "UTF-8";
        res.converterEnc = QStringConverter::Encoding::Utf8;
        res.hasBOM = true;
        return res;
    }
    else if(data.startsWith(QByteArray::fromRawData("\xFF\xFE\x00\x00", 4)))
    {
        qDebug() << "detected UTF-32LE";
        res.encoding = "UTF-32LE";
        res.converterEnc = QStringConverter::Encoding::Utf32LE;
        res.hasBOM = true;
        return res;
    }
    else if(data.startsWith(QByteArray::fromRawData("\xFF\xFE", 2)))
    {
        qDebug() << "detected UTF-16LE";
        res.encoding = "UTF-16LE";
        res.converterEnc = QStringConverter::Encoding::Utf16LE;
        res.hasBOM = true;
        return res;
    }
    else if(data.startsWith(QByteArray::fromRawData("\xFE\xFF", 2)))
    {
        qDebug() << "detected UTF-16BE";
        res.encoding = "UTF-16BE";
        res.converterEnc = QStringConverter::Encoding::Utf16BE;
        res.hasBOM = true;
        return res;
    }
    else if(data.startsWith(QByteArray::fromRawData("\x00\x00\xFE\xFF", 4)))
    {
        qDebug() << "detected UTF-32BE";
        res.encoding = "UTF-32BE";
        res.converterEnc = QStringConverter::Encoding::Utf32BE;
        res.hasBOM = true;
        return res;
    }

    //Try detecting by uchardet.
    uchardet_t detect = uchardet_new();

    uchardet_handle_data(detect, data.constData(), data.size());
    uchardet_data_end(detect);

    QString enc = uchardet_get_encoding(detect, data.size());
    qDebug() << enc;
    if (encodings.contains(enc))
    {
        res.encoding = enc;
        res.converterEnc = encodings[enc];
    }
    else
    {
        // Fallback to UTF-8
        res.encoding = "UTF-8";
        res.converterEnc = encodings["UTF-8"];
    }

    uchardet_delete(detect);

    return res;
}

QString encdetector::detectFileEncoding(const QString &filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly))
        return "Unknown";

    QByteArray data = file.read(1024);
    file.close();

    encodingResult result = detectEncoding(data);
    return result.encoding;
}

QList<QStringConverter::Encoding> encdetector::supportedEncodings()
{
    return
    {
        QStringConverter::Encoding::Utf8,
        QStringConverter::Encoding::Utf16LE,
        QStringConverter::Encoding::Utf16BE,
        QStringConverter::Encoding::Utf32LE,
        QStringConverter::Encoding::Utf32BE,
        QStringConverter::Encoding::Latin1,
        QStringConverter::Encoding::System
    };
}
