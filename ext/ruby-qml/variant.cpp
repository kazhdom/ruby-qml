#include <QVariant>
#include <QDateTime>
#include <QDebug>

extern "C" {

QVariant *qvariant_new()
{
    return new QVariant();
}

QVariant *qvariant_from_int(int value)
{
    return new QVariant(value);
}

QVariant *qvariant_from_float(double value)
{
    return new QVariant(value);
}

QVariant *qvariant_from_boolean(int value)
{
    return new QVariant(bool(value));
}

QVariant *qvariant_from_string(const char *str)
{
    return new QVariant(QString(str));
}

QVariant *qvariant_from_array(int count, QVariant **variants)
{
    QVariantList variantList;
    variantList.reserve(count);
    for (int i = 0; i < count; ++i) {
        variantList << *variants[i];
    }
    return new QVariant(variantList);
}

QVariant *qvariant_from_hash(int count, const char **keys, QVariant **variants)
{
    QVariantHash variantHash;
    variantHash.reserve(count);
    for (int i = 0; i < count; ++i) {
        variantHash[keys[i]] = *variants[i];
    }
    return new QVariant(variantHash);
}

QVariant *qvariant_from_time(int year, int month, int day, int hour, int minute, int second, int msecond, int gmtOffset)
{
    QDateTime dateTime(QDate(year, month, day), QTime(hour, minute, second, msecond));
    dateTime.setUtcOffset(gmtOffset);
    return new QVariant(dateTime);
}

int qvariant_to_int(const QVariant *variant)
{
    return variant->toInt();
}

double qvariant_to_float(const QVariant *variant)
{
    return variant->toDouble();
}

void qvariant_get_string(const QVariant *variant, void (*callback)(const char *))
{
    callback(variant->toString().toUtf8().data());
}

void qvariant_get_array(const QVariant *variant, void (*callback)(QVariant *))
{
    for (const auto &x : variant->toList()) {
        callback(new QVariant(x));
    }
}

void qvariant_get_hash(const QVariant *variant, void (*callback)(const char *, QVariant *))
{
    auto hash = variant->toHash();
    for (auto i = hash.begin(); i != hash.end(); ++i) {
        callback(i.key().toUtf8().data(), new QVariant(i.value()));
    }
}

void qvariant_get_time(const QVariant *variant, int *out)
{
    auto dateTime = variant->toDateTime();
    auto date = dateTime.date();
    auto time = dateTime.time();
    out[0] = date.year();
    out[1] = date.month();
    out[2] = date.day();
    out[3] = time.hour();
    out[4] = time.minute();
    out[5] = time.second();
    out[6] = time.msec();
    out[7] = dateTime.utcOffset();
}

int qvariant_type(const QVariant *variant)
{
    return variant->userType();
}

QVariant *qvariant_convert(const QVariant *variant, int type)
{
    QVariant result = *variant;
    if (!result.convert(type)) {
        return nullptr;
    }
    return new QVariant(result);
}

QVariant *qvariant_dup(const QVariant *variant)
{
    return new QVariant(*variant);
}

void qvariant_destroy(QVariant *variant)
{
    delete variant;
}

}