#include "qrequest.h"
#include <QUrlQuery>

QRequest::QRequest(QString address /*= QString()*/) {
  setAddress(address);
}

QString QRequest::address() const {
  return address_;
}

void QRequest::setAddress(QString address) {
  for (QPair<QString, QString> value : QUrlQuery(QUrl(address)).queryItems())
    addParam(value.first, value.second);
  address_ = address;
}

void QRequest::addParam(QString name, QVariant value) {
  params_[name] = value.toString();
}

bool QRequest::removeParam(QString name) {
  if (false == params_.contains(name))
    return false;
  params_.remove(name);
  return true;
}

QStringList QRequest::paramsNames() const {
  return params_.keys();
}

QMap<QString, QString> QRequest::params() const {
  return params_;
}

QUrl QRequest::url(bool forGetRequest /*= true*/) const {
  QUrl url(address());
  if (forGetRequest)
    url.setQuery(data());
  return url;
}

QNetworkRequest QRequest::request(bool forGetRequest /*= true*/) const {
  QNetworkRequest r(url(forGetRequest));

  if (!forGetRequest) {
    r.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
  }

  return r;
}

QByteArray QRequest::data() const {
  auto b = params_.begin();
  auto e = params_.end();

  QByteArray byteArrayData;

  while (b != e) {
    byteArrayData.append(b.key());
    byteArrayData.append('=');
    byteArrayData.append(b.value());
    byteArrayData.append('&');

    b++;
  }

  byteArrayData.chop(1);

  return byteArrayData;
}
