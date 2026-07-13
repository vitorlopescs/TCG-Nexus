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
    
    // Requisito 3 - Autenticação Forte com Hashing Seguro (RNF 6.7)
    QString authenticateUser(const QString &login, const QString &senha);
    
    // Requisito 4 - Cadastro de Usuários
    bool registerUser(const QString &login, const QString &senha, const QString &perfil);
    
    // Requisito 2 - Métodos do Catálogo e Estoque Local
    QVector<QVariantMap> searchCardsByAttribute(const QString &atributo, const QString &valor);
    bool addCardToStock(const QString &cardId, int quantidade, double preco);
    int stockItemCount();

    // Utilitários de Segurança Corporativa
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