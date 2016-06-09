#pragma once
#include <QObject>
#include <QByteArray>
#include <QNetworkProxy>

#include "qrequest.h"

class QRequestSender : public QObject {
  Q_OBJECT

 public:
  enum RequestError {
    NoError,
    TimeoutError
  };

  QRequestSender(qint64 maxWaitTime = 35000);
  ~QRequestSender();

  void setProxy(const QNetworkProxy& proxy);

  QByteArray get(QRequest& request);
  QByteArray post(QRequest& request);
  QByteArray getWhileSuccess(QRequest& request, int maxCount = 2);
  QByteArray postWhileSuccess(QRequest& request, int maxCount = 2);

  void setMaxWaitTime(qint64 max);

  qint64 maxWaitTime() const;
  RequestError error() const;

 private:
  QByteArray sendRequest(QRequest& request, bool getRequest = true);
  QByteArray sendWhileSuccess(QRequest& request, int maxCount = 2, bool getRequest = true);

 private:
  qint64 max_wait_time_;
  RequestError error_;
  QNetworkProxy proxy_;
};
