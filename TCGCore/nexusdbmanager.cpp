/**
 * @file nexusdbmanager.cpp
 * @brief Implementação de persistência e segurança utilizando hashing PBKDF2 iterativo.
 */

#include "nexusdbmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QStringList>
#include <QRandomGenerator>

bool NexusDbManager::initDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:"); // SQLite embutido em memória para isolamento do MVP
    if (!db.open()) {
        qDebug() << "Erro crítico ao inicializar banco SQLite em memória:" << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    // Alteração BDD: Chave primária é email, inclusão do nome.
    query.exec("CREATE TABLE usuarios ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "email VARCHAR(100) UNIQUE, "
               "nome VARCHAR(100), "
               "senha_hash VARCHAR(128), "
               "salt VARCHAR(64), "
               "perfil VARCHAR(20), "
               "ativo INTEGER DEFAULT 1)");

    query.exec("CREATE TABLE cartas ("
               "id VARCHAR(50) PRIMARY KEY, "
               "name VARCHAR(100), "
               "supertype VARCHAR(50), "
               "type VARCHAR(50), "
               "attacks TEXT, "
               "text TEXT, "
               "evolvesTo VARCHAR(100), "
               "weakness VARCHAR(50), "
               "resistance VARCHAR(50), "
               "rarity VARCHAR(50), "
               "artist VARCHAR(100))");

    query.exec("CREATE TABLE estoque ("
               "carta_id VARCHAR(50) PRIMARY KEY, "
               "quantidade INTEGER, "
               "preco REAL, "
               "FOREIGN KEY(carta_id) REFERENCES cartas(id))");

    // Semente padrão do MVP: Criando conta mestre via e-mail e nome
    registerUser("Administrador Mestre", "admin@tcgnexus.com", "123", "ADMIN");
    return true;
}

QString NexusDbManager::generateSalt() {
    QByteArray bytes;
    for(int i = 0; i < 16; ++i) {
        bytes.append(static_cast<char>(QRandomGenerator::global()->bounded(256)));
    }
    return QString(bytes.toHex());
}

QString NexusDbManager::hashPasswordWithSalt(const QString &senha, const QString &salt) {
    QByteArray derivedKey = senha.toUtf8() + salt.toUtf8();
    for (int i = 0; i < 10000; ++i) {
        derivedKey = QCryptographicHash::hash(derivedKey, QCryptographicHash::Sha512);
    }
    return QString(derivedKey.toHex());
}

bool NexusDbManager::registerUser(const QString &nome, const QString &email, const QString &senha, const QString &perfil) {
    if (nome.isEmpty() || email.isEmpty() || senha.isEmpty()) return false;

    QSqlQuery checa;
    checa.prepare("SELECT COUNT(*) FROM usuarios WHERE email = :email");
    checa.bindValue(":email", email);
    checa.exec();
    if (checa.next() && checa.value(0).toInt() > 0) return false; // E-mail duplicado barrado

    QString salt = generateSalt();
    QString hash = hashPasswordWithSalt(senha, salt);

    QSqlQuery insere;
    insere.prepare("INSERT INTO usuarios (email, nome, senha_hash, salt, perfil) VALUES (:email, :nome, :hash, :salt, :perfil)");
    insere.bindValue(":email", email);
    insere.bindValue(":nome", nome);
    insere.bindValue(":hash", hash);
    insere.bindValue(":salt", salt);
    insere.bindValue(":perfil", perfil);
    return insere.exec();
}

QString NexusDbManager::authenticateUser(const QString &email, const QString &senha) {
    QSqlQuery query;
    query.prepare("SELECT senha_hash, salt, perfil FROM usuarios WHERE email = :email");
    query.bindValue(":email", email);
    query.exec();

    if (query.next()) {
        QString armazenadoHash = query.value(0).toString();
        QString salt = query.value(1).toString();
        QString perfil = query.value(2).toString();

        if (hashPasswordWithSalt(senha, salt) == armazenadoHash) {
            return perfil;
        }
    }
    return QString();
}

QString NexusDbManager::jsonValueToString(const QJsonValue &value) const {
    if (value.isArray()) {
        QStringList partes;
        for (const QJsonValue &item : value.toArray()) {
            if (item.isObject()) {
                QJsonObject obj = item.toObject();
                if (obj.contains("text")) partes << obj.value("text").toString();
                else if (obj.contains("name")) partes << obj.value("name").toString();
            } else {
                partes << item.toString();
            }
        }
        return partes.join("; ");
    }
    return value.toString();
}

int NexusDbManager::ingestCardsFromJson(const QByteArray &jsonData) {
    QJsonParseError erroParse;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &erroParse);
    if (erroParse.error != QJsonParseError::NoError) return -1;

    QJsonArray cartas;
    if (doc.isArray()) cartas = doc.array();
    else if (doc.isObject() && doc.object().contains("data")) cartas = doc.object().value("data").toArray();
    else return -1;

    int importadas = 0;
    for (const QJsonValue &cartaValue : cartas) {
        QJsonObject obj = cartaValue.toObject();
        QSqlQuery insere;
        insere.prepare("INSERT OR REPLACE INTO cartas (id, name, supertype, type, rarity, artist) "
                       "VALUES (:id, :name, :supertype, :type, :rarity, :artist)");
        insere.bindValue(":id", obj.value("id").toString());
        insere.bindValue(":name", obj.value("name").toString());
        insere.bindValue(":supertype", obj.value("supertype").toString());
        insere.bindValue(":type", obj.contains("types") ? jsonValueToString(obj.value("types")) : obj.value("type").toString());
        insere.bindValue(":rarity", obj.value("rarity").toString());
        insere.bindValue(":artist", obj.value("artist").toString());

        if (insere.exec()) importadas++;
    }
    return importadas;
}

int NexusDbManager::ingestCardsFromJsonFile(const QString &filePath) {
    QFile arquivo(filePath);
    if (!arquivo.open(QIODevice::ReadOnly | QIODevice::Text)) return -1;
    QByteArray dados = arquivo.readAll();
    arquivo.close();
    return ingestCardsFromJson(dados);
}

QVector<QVariantMap> NexusDbManager::searchCardsByAttribute(const QString &atributo, const QString &valor) {
    QVector<QVariantMap> resultados;
    static const QStringList colunasPermitidas = {"name", "type", "supertype", "rarity", "artist"};
    QString coluna = colunasPermitidas.contains(atributo) ? atributo : "name";

    QSqlQuery query;
    query.prepare(QString("SELECT id, name, supertype, type, rarity, artist FROM cartas WHERE %1 LIKE :valor").arg(coluna));
    query.bindValue(":valor", "%" + valor + "%");
    query.exec();

    while (query.next()) {
        QVariantMap item;
        item["id"] = query.value("id");
        item["name"] = query.value("name");
        item["supertype"] = query.value("supertype");
        item["type"] = query.value("type");
        item["rarity"] = query.value("rarity");
        item["artist"] = query.value("artist");
        resultados.append(item);
    }
    return resultados;
}

bool NexusDbManager::addCardToStock(const QString &cardId, int quantidade, double preco) {
    QSqlQuery insere;
    insere.prepare("INSERT OR REPLACE INTO estoque (carta_id, quantidade, preco) VALUES (:id, :qtd, :preco)");
    insere.bindValue(":id", cardId);
    insere.bindValue(":qtd", quantidade);
    insere.bindValue(":preco", preco);
    return insere.exec();
}

bool NexusDbManager::updateCardPrice(const QString &cardId, double novoPreco) {
    if(novoPreco < 0) return false;
    
    QSqlQuery update;
    update.prepare("UPDATE estoque SET preco = :preco WHERE carta_id = :id");
    update.bindValue(":preco", novoPreco);
    update.bindValue(":id", cardId);
    return update.exec();
}

int NexusDbManager::stockItemCount() {
    QSqlQuery query("SELECT COUNT(*) FROM estoque");
    if (query.next()) return query.value(0).toInt();
    return 0;
}

bool NexusDbManager::checkEmailExists(const QString &email) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM usuarios WHERE email = :email");
    query.bindValue(":email", email);
    query.exec();
    return query.next() && query.value(0).toInt() > 0;
}

bool NexusDbManager::deactivateUser(int idTarget, int idLogado) {
    if (idTarget == idLogado) return false; // Regra BDD: Não inativar a si mesmo
    
    QSqlQuery query;
    query.prepare("UPDATE usuarios SET ativo = 0 WHERE id = :id");
    query.bindValue(":id", idTarget);
    return query.exec();
}

int NexusDbManager::getLoggedUserId(const QString &email) {
    QSqlQuery query;
    query.prepare("SELECT id FROM usuarios WHERE email = :email");
    query.bindValue(":email", email);
    if(query.exec() && query.next()) return query.value(0).toInt();
    return -1;
}

QVector<QVariantMap> NexusDbManager::getAllUsers() {
    QVector<QVariantMap> lista;
    QSqlQuery query("SELECT id, nome, email, perfil, ativo FROM usuarios");
    while(query.next()) {
        QVariantMap u;
        u["id"] = query.value("id");
        u["nome"] = query.value("nome");
        u["email"] = query.value("email");
        u["perfil"] = query.value("perfil");
        u["ativo"] = query.value("ativo");
        lista.append(u);
    }
    return lista;
}

bool NexusDbManager::updateUserProfile(int idTarget, const QString &novoPerfil) {
    QSqlQuery query;
    query.prepare("UPDATE usuarios SET perfil = :perfil WHERE id = :id");
    query.bindValue(":perfil", novoPerfil);
    query.bindValue(":id", idTarget);
    return query.exec();
}