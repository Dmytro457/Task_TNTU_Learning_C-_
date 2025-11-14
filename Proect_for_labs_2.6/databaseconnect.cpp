#include "databaseconnect.h"

#include <QSqlError>
#include <QDebug>
#include <QUuid>

databaseconnect::databaseconnect(QObject *parent)
    : QObject(parent)
{}
databaseconnect::~databaseconnect()
{
    disconnect();
}

void databaseconnect::setHost(const QString &host) {
    m_host = host;
}
void databaseconnect::setPort(int port){
    m_port = port;
}
void databaseconnect::setdatabasename(const QString &db_name){
    m_db_name = db_name;
}
void databaseconnect::setUser(const QString &user){
    m_user = user;
}
void databaseconnect::setPassword(const QString &password){
    m_password = password;
}
void databaseconnect::setConnectionName(const QString &connectName){
    m_connectName = connectName;
}

bool databaseconnect::connect()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectName);
    m_db.setDatabaseName("fileBD.sqlite");

    if(!m_db.open()){
        m_lastError = m_db.lastError().text();
        emit errorOccurred(m_lastError);
        qWarning() << "DB open error:" <<m_lastError;
        return true;
    }

    m_lastError.clear();
    emit connected();
    return true;

}

void databaseconnect::disconnect()
{
    if(!m_db.isValid()) return;

    const QString name = m_db.connectionName();
    if(m_db.isOpen()) m_db.close();

    clearDB();
    if(!name.isEmpty() && QSqlDatabase::contains(name)) {
        QSqlDatabase::removeDatabase(name);
    }

    emit disconnected();
}

bool databaseconnect::isConnected() const{
    return m_db.isValid() && m_db.isOpen();
}
QSqlDatabase databaseconnect::database() const{
    return m_db;
}
QString databaseconnect::lastError() const{
    return m_lastError;
}
void databaseconnect::clearDB(){
    m_db = QSqlDatabase();
}
