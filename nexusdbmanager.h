#ifndef NEXUSDBMANAGER_H
#define NEXUSDBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QVariant>
#include <QJsonValue>
#include <QDebug>
#include "tcgcore_global.h"

/**
 * @file nexusdbmanager.h
 * @brief Camada de persistência do TCG Nexus: usuários, catálogo de cartas e estoque local.
 */

/**
 * @class NexusDbManager
 * @brief Implementa o Padrão de Projeto Singleton para gerenciar a camada de Persistência do TCG Nexus.
 *
 * Consolida, em um único ponto de acesso ao SQLite, as responsabilidades de dados
 * previstas na Especificação de Requisitos (DER v3.0):
 *  - Ingestão de metadados oficiais de cartas via JSON (Requisito 1 - módulo DevPortal).
 *  - Cadastro e autenticação de usuários com senha hasheada (Requisitos 3 e 4).
 *  - Consulta ao catálogo por atributo técnico e gestão de estoque local (Requisito 2 - módulo StoreDash).
 *
 * Por ser exportada de uma DLL (TCGCORE_EXPORT), pode ser consumida por qualquer
 * executável cliente (ex: TCGGUI) sem duplicar a lógica de acesso a dados.
 */
class TCGCORE_EXPORT NexusDbManager {
public:
    /**
     * @brief Retorna a instância única do gerenciador de banco de dados (Singleton).
     * @return NexusDbManager& Referência para a instância estática única.
     */
    static NexusDbManager& getInstance() {
        static NexusDbManager instance;
        return instance;
    }

    /**
     * @brief Inicializa o banco SQLite em memória e cria as tabelas do sistema
     * (usuarios, cartas, estoque), semeando um usuário "admin"/"123" para
     * viabilizar o primeiro acesso e o cadastro dos demais usuários.
     * @return bool true se o banco foi aberto e as tabelas criadas com sucesso.
     */
    bool initDatabase();

    /**
     * @brief Lê um arquivo em disco e delega a ingestão para ingestCardsFromJson().
     * @param filePath Caminho absoluto/relativo do arquivo JSON de cartas.
     * @return int Quantidade de cartas importadas, ou -1 em caso de falha de leitura/parse.
     */
    int ingestCardsFromJsonFile(const QString &filePath);

    /**
     * @brief Realiza o parse de um JSON de cartas (formato Pokémon TCG) e popula a
     * tabela "cartas" (Requisito 1). Aceita tanto um array raiz quanto um objeto
     * com a chave "data" (formato usado pela API oficial do Pokémon TCG).
     * @param jsonData Conteúdo bruto do arquivo JSON.
     * @return int Quantidade de cartas importadas com sucesso, ou -1 se o JSON for inválido.
     */
    int ingestCardsFromJson(const QByteArray &jsonData);

    /**
     * @brief Valida as credenciais de um usuário comparando o hash da senha informada
     * com o hash armazenado no SQLite (Requisito 3).
     * @param login Nome de usuário informado na tela de login.
     * @param senha Senha em texto puro informada na tela de login.
     * @return QString O perfil do usuário ("ADMIN" ou "LOJISTA") em caso de sucesso;
     * string vazia caso as credenciais sejam inválidas.
     */
    QString authenticateUser(const QString &login, const QString &senha);

    /**
     * @brief Cadastra um novo usuário no sistema, armazenando a senha já hasheada (Requisito 4).
     * @param login Nome de usuário (chave única).
     * @param senha Senha em texto puro (será hasheada antes de persistir).
     * @param perfil Perfil de acesso do usuário ("ADMIN" ou "LOJISTA" - Requisito 5).
     * @return bool true se o cadastro foi criado; false se o login já existir.
     */
    bool registerUser(const QString &login, const QString &senha, const QString &perfil);

    /**
     * @brief Busca cartas no catálogo por um atributo técnico específico (Requisito 2).
     * @param atributo Nome da coluna a filtrar (name, type, supertype, rarity ou artist).
     * @param valor Valor (ou trecho) a ser buscado, com correspondência parcial (LIKE).
     * @return QVector<QVariantMap> Lista de cartas encontradas, cada uma como um mapa de atributos.
     */
    QVector<QVariantMap> searchCardsByAttribute(const QString &atributo, const QString &valor);

    /**
     * @brief Adiciona (ou atualiza) uma carta no estoque local do lojista (Requisito 2).
     * @param cardId Identificador da carta (deve existir na tabela "cartas").
     * @param quantidade Unidades disponíveis em estoque.
     * @param preco Preço de venda definido pelo lojista.
     * @return bool true se a operação foi persistida com sucesso.
     */
    bool addCardToStock(const QString &cardId, int quantidade, double preco);

    /**
     * @brief Retorna a quantidade de itens distintos cadastrados no estoque local.
     * @return int Número de linhas da tabela "estoque".
     */
    int stockItemCount();

    /**
     * @brief Gera o hash SHA-256 de uma senha em texto puro.
     *
     * Escolha de MVP: SHA-256 via QCryptographicHash (nativo do Qt, sem dependências
     * externas pesadas). O RNF 6.7 do DER recomenda bcrypt/Argon2 para produção;
     * fica registrado aqui como evolução pós-MVP.
     *
     * @param senha Senha em texto puro.
     * @return QString Hash hexadecimal da senha.
     */
    static QString hashPassword(const QString &senha);

private:
    QSqlDatabase db; ///< Conexão ativa com o banco de dados SQLite em memória.

    /** @brief Construtor privado (Regra do Singleton): impede instanciação externa. */
    NexusDbManager() {}

    /** @brief Construtor de cópia deletado: impede múltiplas instâncias. */
    NexusDbManager(const NexusDbManager&) = delete;

    /** @brief Operador de atribuição deletado: impede múltiplas instâncias. */
    void operator=(const NexusDbManager&) = delete;

    /**
     * @brief Converte um QJsonValue (string simples ou array de strings/objetos)
     * em uma única QString legível, unindo múltiplos valores com "; ".
     * Usado para normalizar campos como "attacks", "weaknesses" e "evolvesTo",
     * que no dataset oficial do Pokémon TCG podem vir como arrays.
     * @param value Valor JSON de origem.
     * @return QString Representação textual normalizada (vazia se o valor for nulo).
     */
    QString jsonValueToString(const QJsonValue &value) const;
};

#endif // NEXUSDBMANAGER_H
