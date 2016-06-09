#include "qrequestsender.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QThread>
#include <QTimer>

QRequestSender::QRequestSender(qint64 maxWaitTime /*= 35000*/) {
  setMaxWaitTime(maxWaitTime);
  error_ = NoError;
}

QRequestSender::~QRequestSender() {

}

void QRequestSender::setProxy(const QNetworkProxy& proxy) {
  proxy_ = proxy;
}

QByteArray QRequestSender::get(QRequest& request) {
  return sendRequest(request, true);
}

QByteArray QRequestSender::post(QRequest& request) {
  return sendRequest(request, false);
}

QByteArray QRequestSender::getWhileSuccess(QRequest& request, int maxCount /*= 2*/) {
  return sendWhileSuccess(request, maxCount, true);
}

QByteArray QRequestSender::postWhileSuccess(QRequest& request, int maxCount /*= 2*/) {
  return sendWhileSuccess(request, maxCount, false);
}

void QRequestSender::setMaxWaitTime(qint64 max) {
  max_wait_time_ = max;
}

qint64 QRequestSender::maxWaitTime() const {
  return max_wait_time_;
}

QRequestSender::RequestError QRequestSender::error() const {
  return error_;
}

QByteArray QRequestSender::sendRequest(QRequest& request, bool getRequest /*= true*/) {
  QTimer timer;
  timer.setInterval(max_wait_time_);
  timer.setSingleShot(true);

  QEventLoop loop;
  QSharedPointer<QNetworkAccessManager> manager(new QNetworkAccessManager);
  manager->setProxy(proxy_);

  QNetworkReply* reply = getRequest ? manager->get(request.request()) :
    manager->post(request.request(false), request.data());

#if defined(NETWORK_SHOW_SEND_REQUESTS)
  if (getRequest)
    qDebug() << "[GET] " << request.request().url().toString();
  else
    qDebug() << "[POST]" << request.request(false).url().toString() << request.data();
#endif

  QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  QObject::connect(&timer, &QTimer::timeout, reply, &QNetworkReply::abort);

  timer.start();
  loop.exec();

  QByteArray data;

  if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
    data = reply->readAll();
    error_ = QRequestSender::NoError;
  } else {
    error_ = QRequestSender::TimeoutError;
  }

  reply->deleteLater();

#if defined(NETWORK_SHOW_SEND_REQUESTS)
  qDebug() << "[ANSWER]" << data;
#endif

  return data;
}

QByteArray QRequestSender::sendWhileSuccess(QRequest& request, int maxCount /*= 2*/, bool getRequest /*= true*/) {
  if (maxCount < 0)
    throw QString(__LINE__ + " " __FILE__);

  int c = 0;
  QByteArray answer;

  while (c < maxCount) {
    c++;
    answer = getRequest ? get(request) : post(request);

    if (error() == NoError)
      break;

    qDebug() << "Ошибка при отправке запроса. Код ошибки - " << error() << ". Повторная отправка запроса через 2 секунды\n";
    QThread::currentThread()->msleep(2000);
  }

  return answer;
}
