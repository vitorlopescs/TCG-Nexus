/**
 * @file nexusdbmanager.h
 * @brief Gerenciador de persistência de dados em conformidade com o RNF 6.7 (macOS/Multiplataforma).
 * @class NexusDbManager
 * @brief Singleton responsável pelas regras de dados e segurança do TCG Nexus.
 */

#ifndef NEXUSDBMANAGER_H
#define NEXUSDBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QVariantMap>
#include <QJsonValue>
#include <QDebug>
#include "tcgcore_global.h"

class TCGCORE_EXPORT NexusDbManager {
public:
    /**
     * @brief Retorna a instância única do gerenciador (Padrão Singleton).
     */
    static NexusDbManager& getInstance() {
        static NexusDbManager instance;
        return instance;
    }

    bool initDatabase();
    int ingestCardsFromJsonFile(const QString &filePath);
    int ingestCardsFromJson(const QByteArray &jsonData);
    
    QString authenticateUser(const QString &email, const QString &senha);
    bool registerUser(const QString &nome, const QString &email, const QString &senha, const QString &perfil);
    
    QVector<QVariantMap> searchCardsByAttribute(const QString &atributo, const QString &valor);
    bool addCardToStock(const QString &cardId, int quantidade, double preco);
    bool updateCardPrice(const QString &cardId, double novoPreco);
    int stockItemCount();

    bool checkEmailExists(const QString &email);
    bool deactivateUser(int idTarget, int idLogado);
    bool updateUserProfile(int idTarget, const QString &novoPerfil);
    int getLoggedUserId(const QString &email);
    QVector<QVariantMap> getAllUsers();

    static QString generateSalt();
    static QString hashPasswordWithSalt(const QString &senha, const QString &salt);

private:
    QSqlDatabase db;
    NexusDbManager() {}
    NexusDbManager(const NexusDbManager&) = delete;
    void operator=(const NexusDbManager&) = delete;
    QString jsonValueToString(const QJsonValue &value) const;
};

#endif // NEXUSDBMANAGER_H