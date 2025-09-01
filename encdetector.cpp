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

encdetector::encodingResult encdetector::detectEncoding(const QByteArray &data)
{
    encdetector::encodingResult res;
    res.hasBOM = false;

    //Try detecting by BOM.
    if(data.startsWith("\xEF\xBB\xBF"))
    {
        res.encoding = "UTF-8";
        res.converterEnc = QStringConverter::Encoding::Utf8;
        res.hasBOM = true;
        return res;
    }
    else if(data.startsWith("\xFF\xFE"))
    {
        res.encoding = "UTF-16LE";
        res.converterEnc = QStringConverter::Encoding::Utf16LE;
        res.hasBOM = true;
        return res;
    }
    else if(data.startsWith("\xFE\xFF"))
    {
        res.encoding = "UTF-16BE";
        res.converterEnc = QStringConverter::Encoding::Utf16BE;
        res.hasBOM = true;
        return res;
    }
    else if(data.startsWith("\xFF\xFE\x00"))
    {
        res.encoding = "UTF-32LE";
        res.converterEnc = QStringConverter::Encoding::Utf32LE;
        res.hasBOM = true;
        return res;
    }
    else if(data.startsWith("\x00\x00\xFE\xFF"))
    {
        res.encoding = "UTF-32BE";
        res.converterEnc = QStringConverter::Encoding::Utf32BE;
        res.hasBOM = true;
        return res;
    }

    //Try detecting by content.
    auto decoder = QStringDecoder(QStringConverter::Encoding::Utf8);
    QString test = decoder.decode(data);
    if (decoder.hasError())
    {
        QList<QStringConverter::Encoding> encToTest
        {
            QStringConverter::Encoding::Latin1,
            QStringConverter::Encoding::System,
            QStringConverter::Encoding::Utf16LE,
            QStringConverter::Encoding::Utf16BE,
            QStringConverter::Encoding::Utf32LE,
            QStringConverter::Encoding::Utf32BE
        };

        for (auto enc : encToTest)
        {
            decoder = QStringDecoder(enc);
            test = decoder.decode(data);
            if (!decoder.hasError())
            {
                switch (enc) {
                case QStringConverter::Encoding::Latin1:
                    res.encoding = "ISO-8859-1";
                    break;
                case QStringConverter::Encoding::System:
                    res.encoding = "System";
                    break;
                case QStringConverter::Encoding::Utf16LE:
                    res.encoding = "UTF-16LE";
                    break;
                case QStringConverter::Encoding::Utf16BE:
                    res.encoding = "UTF-16BE";
                    break;
                case QStringConverter::Encoding::Utf32LE:
                    res.encoding = "UTF-32LE";
                    break;
                case QStringConverter::Encoding::Utf32BE:
                    res.encoding = "UTF-32BE";
                    break;
                default:
                    res.encoding = "Unknown";
                    break;
                }
                res.converterEnc = enc;
                return res;
            }
        }

        res.encoding = "ISO-8859-1";
        res.converterEnc = QStringConverter::Encoding::Latin1;
    }
    else
    {
        res.encoding = "UTF-8";
        res.converterEnc = QStringConverter::Encoding::Utf8;
    }

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
        QStringConverter::Encoding::Utf32LE,
        QStringConverter::Encoding::Latin1,
        QStringConverter::Encoding::System
    };
}
