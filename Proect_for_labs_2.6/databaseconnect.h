#ifndef DATABASECONNECT_H
#define DATABASECONNECT_H

#include <QObject>
#include <QSqlDatabase>

class databaseconnect : public QObject
{
    Q_OBJECT
public:
    explicit databaseconnect(QObject *parent = nullptr);
    ~databaseconnect();

    void setHost(const QString &host);
    void setPort(int port);
    void setdatabasename(const QString &db_name);
    void setUser(const QString &user);
    void setPassword(const QString &password);
    void setConnectionName(const QString &connectName);

    bool connect();
    void disconnect();
    bool isConnected() const;
    QSqlDatabase database() const;
    QString lastError() const;
signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString &message);

private:
    QString m_host;
    int m_port = 3306;
    QString m_db_name;
    QString m_user;
    QString m_password;
    QString m_connectName;

    QSqlDatabase m_db;
    QString m_lastError;
    void clearDB();
};

#endif // DATABASECONNECT_H
