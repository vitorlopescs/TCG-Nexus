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
     * @return Referência para a instância de NexusDbManager.
     */
    static NexusDbManager& getInstance() {
        static NexusDbManager instance;
        return instance;
    }

    /**
     * @brief Inicializa o banco de dados SQLite em memória e cria as tabelas padrão.
     * @return true se o banco foi inicializado com sucesso, false caso contrário.
     */
    bool initDatabase();

    /**
     * @brief Lê e ingere um arquivo JSON contendo dados das cartas oficiais.
     * @param filePath Caminho completo para o arquivo no sistema operacional.
     * @return Quantidade de cartas inseridas ou -1 em caso de erro.
     */
    int ingestCardsFromJsonFile(const QString &filePath);

    /**
     * @brief Processa os dados brutos em formato JSON das cartas.
     * @param jsonData Array de bytes com o conteúdo do JSON.
     * @return Quantidade de cartas importadas.
     */
    int ingestCardsFromJson(const QByteArray &jsonData);
    
    /**
     * @brief Autentica um usuário no sistema validando seu e-mail e senha.
     * @param email E-mail do usuário.
     * @param senha Senha em texto plano.
     * @return QString contendo o perfil ("ADMIN" ou "LOJISTA") ou vazio em caso de falha.
     */
    QString authenticateUser(const QString &email, const QString &senha);
    
    /**
     * @brief Registra um novo usuário no banco de dados.
     * @param nome Nome completo do usuário.
     * @param email E-mail único do usuário.
     * @param senha Senha em texto plano.
     * @param perfil Perfil de acesso ("ADMIN" ou "LOJISTA").
     * @return true se o cadastro ocorreu com sucesso, false se o e-mail já existir.
     */
    bool registerUser(const QString &nome, const QString &email, const QString &senha, const QString &perfil);
    
    /**
     * @brief Busca cartas no catálogo baseadas em um atributo específico.
     * @param atributo Nome da coluna (ex: name, type, rarity).
     * @param valor Valor ou trecho para pesquisar (LIKE %valor%).
     * @return Vetor de mapas contendo os dados das cartas encontradas.
     */
    QVector<QVariantMap> searchCardsByAttribute(const QString &atributo, const QString &valor);

    /**
     * @brief Adiciona uma quantidade específica de uma carta ao estoque local.
     * @param cardId ID oficial da carta.
     * @param quantidade Número de unidades.
     * @param preco Preço de venda.
     * @return true se a operação de inserção foi bem-sucedida.
     */
    bool addCardToStock(const QString &cardId, int quantidade, double preco);

    /**
     * @brief Atualiza o preço de uma carta já presente no estoque.
     * @param cardId ID oficial da carta.
     * @param novoPreco Novo valor de venda (deve ser positivo).
     * @return true se o preço foi atualizado com sucesso.
     */
    bool updateCardPrice(const QString &cardId, double novoPreco);

    /**
     * @brief Retorna a quantidade de tipos de cartas diferentes cadastradas no estoque.
     * @return Quantidade em formato inteiro.
     */
    int stockItemCount();

    /**
     * @brief Gera um Salt criptográfico de 16 bytes convertido para Hexadecimal.
     * @return QString contendo o Salt gerado.
     */
    static QString generateSalt();

    /**
     * @brief Gera um hash PBKDF2 iterativo baseado em SHA-512 utilizando uma senha e um Salt.
     * @param senha Senha em texto plano.
     * @param salt Salt associado ao usuário.
     * @return QString contendo o Hash resultante.
     */
    static QString hashPasswordWithSalt(const QString &senha, const QString &salt);

private:
    QSqlDatabase db;
    NexusDbManager() {}
    NexusDbManager(const NexusDbManager&) = delete;
    void operator=(const NexusDbManager&) = delete;

    /**
     * @brief Converte valores genéricos de JSON (como arrays) para uma string plana.
     * @param value Referência para o QJsonValue.
     * @return QString processada separada por ponto e vírgula.
     */
    QString jsonValueToString(const QJsonValue &value) const;
};

#endif // NEXUSDBMANAGER_H