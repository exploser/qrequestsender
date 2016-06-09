#pragma once
#include <QString>
#include <QMap>
#include <QVariant>
#include <QNetworkRequest>

class QRequest {
 public:
  QRequest(QString address = QString());

  QString address() const;
  void setAddress(QString address);

  void addParam(QString name, QVariant value);
  bool removeParam(QString name);

  QStringList paramsNames() const;
  QMap<QString, QString> params() const;

  QUrl url(bool withParams = true) const;
  QNetworkRequest request(bool withParams = true) const;
  QByteArray data() const;

 private:
  QString address_;
  QMap<QString, QString> params_;
};
