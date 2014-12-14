/*
 * Copyright 2014 Kevin Brightwell <kevin.brightwell2@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "router/QWebRoute.h"

#include <QString>
#include <QStringBuilder>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QStack>
#include <QStringList>
#include <QDebug>
#include <QHash>

class QWebRoute_Regex : public QObject, public QWebRoute {

    Q_OBJECT

public:

    QWebRoute_Regex(const QRegularExpression &route,
                     QObject *parent = nullptr)
        : QObject(parent), QWebRoute(route.pattern()), m_urlPattern(route) {
#if QT_VERSION >= 0x054000
        // if on Qt 5.4+ we can optimize the regex
        m_urlPattern.optimize();
#endif
    }

    /*!
     * Return a \ref QList of variables that are pulled from a route after
     * parsing. The order is **not** guarenteed.
     * \return const reference to the variables
     */
    inline
    const QStringList variables() const {
        return m_urlPattern.namedCaptureGroups();
    }

    /**
     * @brief checkPath use the QRegularExpression module to compute
     * @param paths
     * @return
     */
    virtual
    ParsedRoute::Ptr const checkPath(const QString path) {
        typedef QRegularExpression QRE;

        QRegularExpressionMatch match = m_urlPattern.match(path, 0, QRE::NormalMatch, QRE::AnchoredMatchOption);

        // check if there was a match, if there is then make sure that it was the entire
        // expression, not part of it
        if (!match.hasMatch()) {
            if (match.hasPartialMatch()) {
                qDebug() << "QWebRoute::checkPath(" << path << "): No match was found using '" << m_urlPattern.pattern() << "', but did find partial match.";
            }
            
            return ParsedRoute::Ptr();
        }

        QHash<QString, QString> namedVars;
        QStringList splats;
        const QStringList names = m_urlPattern.namedCaptureGroups();
        const QStringList vals = match.capturedTexts();
        for (int i = 1; i < names.size(); ++i) {
            const QString &name = names[i];
            const QString &val = vals[i];

            if (!val.isNull()) {
                if (name.isEmpty()) {
                    // name is invalid:
                    splats += val;
                } else {
                    namedVars[name] = val;
                }
            }
        }

        return ParsedRoute::Ptr(new ParsedRoute(namedVars, splats, vals));
    }

private:
    const QRegularExpression m_urlPattern;

};


QWebRoute::Ptr QWebRouteFactory::createRegex(const QString &comp) const {    
    return createRegex(QRegularExpression(comp));
}

QWebRoute::Ptr QWebRouteFactory::createRegex(const QRegularExpression &regex) const {
    
    clearError();
    
    if (regex.pattern().endsWith('/') || regex.pattern().endsWith("/$")) {
        setError(QWebRouteFactory::SLASH_TERMINATOR, "Path can not end with /, recieved: " % regex.pattern());
        
        return QWebRoute::Ptr();
    }
    
    return QWebRoute::Ptr(new QWebRoute_Regex(regex));
}

// TODO Replace with boost::spirit
static
const QString VALID_OPT_CHARS("[\\w\\d\\-_\\*\\+\\.]");

const QString VALID_NAME_CHARS("[\\w\\d\\-_]");

/**
 * @brief RE_PATH Regex for path matching, Perl regex:
 *      `^(?::(?<name>[\w\d\-_]+)\$?)?(?:([\w\d\-_\.\*]+)(?:\|([\w\d\-_\.\*]+))*)?$`
 */
static
const QRegularExpression RE_PATH("^(?::(?<name>" % VALID_NAME_CHARS % "+)\\$?)?" %
                                 "(?:(" % VALID_OPT_CHARS % "+)(?:\\|(" % VALID_OPT_CHARS % "+))*)?$");

/**
 * Replace needle with `with` within haystack. 
 */
QString replaceRE(const QRegularExpression &needle, const QString &haystack, const QString &with) {

    QString out = haystack;
    QRegularExpressionMatchIterator iter = needle.globalMatch(out);
    QStack<QRegularExpressionMatch> matches;
    while (iter.hasNext()) {
        matches.push(iter.next());
    }
    
    while (!matches.isEmpty()) {
        const auto m = matches.pop();
        out = out.replace(m.capturedStart(1), m.capturedLength(1), with);
    }
    
    return out;
}

static
QString convertPathSyntax(const QString &path, QWebRouteFactory::CreationError * const error) {
    
    const static QString WILDCARD_CHARS = "(" % VALID_NAME_CHARS % "+)";
    const static QRegularExpression WILDCARD_STAR("[^\\\\](\\*)");
    const static QRegularExpression WILDCARD_PLUS("[^\\\\](\\+)");

    typedef QRegularExpression QRE;

    if (!path.startsWith('/')) {
        *error = QWebRouteFactory::ROOT_MISSING;
        return QString("Path Error: Path did not start with /");
    }
    
    if (path.endsWith('/')) {
        *error = QWebRouteFactory::SLASH_TERMINATOR;
        return "Path Error: Path ends with /";
    }

    const QStringList pathParts = path.split('/', QString::SkipEmptyParts);

    QStringList outParts;

    QStringList badParts;

    for (QString part : pathParts) {
        QRegularExpressionMatch match = RE_PATH.match(part, 0,
                                                      QRE::PartialPreferCompleteMatch,
                                                      QRE::AnchoredMatchOption);

        if (!match.hasMatch()) {
            badParts += part;
        }

        if (badParts.size() > 0) {
            // just looking for syntax errors now

            continue;
        }

        QString buff;
        buff.reserve(part.size() * 2);

        bool groupStarted = false; // true when we started a RegEx group

        // check if this got named
        const QString name = match.captured("name");

        if (!name.isNull()) {
            buff += "(?<" % name % '>';
            groupStarted = true;
        }

        if (match.capturedTexts().size() > 2) {
            // create a regex OR

            // skip two because first is whole match, second is name (which
            // may or may not be present)
            const QStringList OROpts = match.capturedTexts().mid(2);

            static const QRegularExpression GROUP_REQ("(?:[^\\\\][\\*\\+]|^[\\*\\+])");
            // check if we need to create a non-capturing group because of
            // multiple options that are valid.
            if (!groupStarted) {
                if (GROUP_REQ.match(part).hasMatch()) {
                    buff += "(";
                    groupStarted = true;
                } else if (OROpts.size() > 1) {
                    buff += "(?:";
                    groupStarted = true;
                }
            } 
            
            static const QRegularExpression WILDCARD_STAR_GT2("\\*{2,}");
            static const QRegularExpression WILDCARD_PLUS_GT2("\\+{2,}");

            // for the OR options, replace wild cards if needed
            for (QString opt : OROpts) {
                opt = opt.replace('.', "\\.");
                
                opt = replaceRE(WILDCARD_STAR_GT2, opt, "*");
                opt = replaceRE(WILDCARD_PLUS_GT2, opt, "+");
                
                opt = replaceRE(WILDCARD_STAR, opt, VALID_NAME_CHARS % '*');
                if (opt.startsWith('*')) {
                    opt = opt.replace(0, 1, VALID_NAME_CHARS % '*');
                }
                
                opt = replaceRE(WILDCARD_PLUS, opt, VALID_NAME_CHARS % '+');
                if (opt.startsWith('+')) {
                    opt = opt.replace(0, 1, VALID_NAME_CHARS % '+');
                }
       
                buff += opt % '|';
            }

            // take off the last |
            buff.chop(1);
        } else {
            // there was no char placed, need to insert a wildcard:
            buff += VALID_NAME_CHARS % "+";
        }

        if (groupStarted) {
            // need to close the grouping
            buff += ')';
        }

        buff.squeeze(); // free extra memory

        outParts += buff;
    }

    if (badParts.size() > 0) {
        *error = QWebRouteFactory::PATH_PART_ERROR; // failed
        return QString("Syntax failed to parse the following components:\n\t\t") %
                    badParts.join("\n\t\t");
    }

    *error = QWebRouteFactory::NO_ERROR;
    QString out = "^/" % outParts.join('/') % "$";

    return out;
}

QWebRoute::Ptr QWebRouteFactory::create(const QString &route) const {
    // create a QRegularExpression from the custom dsl:
    CreationError error = NO_ERROR;
    QString expr = convertPathSyntax(route, &error);

    if (error != NO_ERROR) {
        setError(error, expr);

        return QWebRoute::Ptr();
    }

    QRegularExpression re(expr);

    if (!re.isValid()) {
        setError(INVALID_REGEX_PRODUCED, "Invalid Regex Produced (error index: " % QString::number(re.patternErrorOffset()) % "), {re = " % re.pattern() % ", route = " % route % '}');

        return QWebRoute::Ptr();
    }

    qDebug() << "Parsed: '" << route << "' -> '" << re.pattern() << '\'';

    clearError();
    return QWebRoute::Ptr(new QWebRoute_Regex(re));
}

#include "QWebRoute.moc"
