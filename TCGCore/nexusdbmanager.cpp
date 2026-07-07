/**
 * @file nexusdbmanager.cpp
 * @brief Implementação da camada de persistência do TCG Nexus.
 */

#include "nexusdbmanager.h"

#include <QFile>
#include <QIODevice>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QCryptographicHash>
#include <QStringList>

//Inicializar o banco de dados
bool NexusDbManager::initDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    //Cria um banco de dados na memoria RAM
    db.setDatabaseName(":memory:");

    if (!db.open()) {
        qDebug() << "Erro ao conectar no BD:" << db.lastError().text();
        return false;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE usuarios ("
               "login VARCHAR(50) PRIMARY KEY, "
               "senha_hash VARCHAR(128), "
               "perfil VARCHAR(20))");

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

    // Seed de MVP: usuário administrador padrão para viabilizar o primeiro
    // acesso e o cadastro dos demais usuários (ex.: um Lojista) via DevPortal.
    registerUser("admin", "123", "ADMIN");

    return true;
}

//Transforma a senha em um codigo embaralhado e irreversivel (banco de dados)
QString NexusDbManager::hashPassword(const QString &senha) {
    return QString(QCryptographicHash::hash(senha.toUtf8(), QCryptographicHash::Sha256).toHex());
}

//Registra usuario
bool NexusDbManager::registerUser(const QString &login, const QString &senha, const QString &perfil) {
    //Cria uma consulta para verificar se usuario ja existe
    QSqlQuery checa;
    //Retorna numero de linhas que os logins sao iguais ao que acabou de registrar
    checa.prepare("SELECT COUNT(*) FROM usuarios WHERE login = :login");
    //Coloco o login que quero saber
    checa.bindValue(":login", login);
    //Vejo quantos existem
    checa.exec();
    //Se for maior que zero, ja existe o usuario
    if (checa.next() && checa.value(0).toInt() > 0) {
        return false; // usuario ja existe
    }

    //Cria uma consulta para inserção
    QSqlQuery insere;
    insere.prepare("INSERT INTO usuarios (login, senha_hash, perfil) VALUES (:login, :hash, :perfil)");
    insere.bindValue(":login", login);
    insere.bindValue(":hash", hashPassword(senha));
    insere.bindValue(":perfil", perfil);
    return insere.exec();
}

//Autentica usuario
QString NexusDbManager::authenticateUser(const QString &login, const QString &senha) {
    //Cria uma consulta
    QSqlQuery query;
    query.prepare("SELECT perfil FROM usuarios WHERE login = :login AND senha_hash = :hash");
    query.bindValue(":login", login);
    query.bindValue(":hash", hashPassword(senha));
    query.exec();

    if (query.next()) {
        return query.value(0).toString();
    }
    return QString();
}

//Converte de JSON para String
QString NexusDbManager::jsonValueToString(const QJsonValue &value) const {
    if (value.isArray()) {
        QStringList partes;
        for (const QJsonValue &item : value.toArray()) {
            if (item.isObject()) {
                QJsonObject obj = item.toObject();
                if (obj.contains("text")) partes << obj.value("text").toString();
                else if (obj.contains("type")) partes << obj.value("type").toString();
                else if (obj.contains("name")) partes << obj.value("name").toString();
            } else {
                partes << item.toString();
            }
        }
        return partes.join("; ");
    }
    if (value.isString()) {
        return value.toString();
    }
    return QString(); // valor nulo/ausente -> string vazia
}

//Inserir cartas no banco de dados
int NexusDbManager::ingestCardsFromJson(const QByteArray &jsonData) {
    QJsonParseError erroParse;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &erroParse);

    //Arquivo corrompido
    if (erroParse.error != QJsonParseError::NoError) {
        qDebug() << "Erro ao parsear JSON:" << erroParse.errorString();
        return -1;
    }

    //Verifico se é array ou apenas um objeto
    QJsonArray cartas;
    if (doc.isArray()) {
        cartas = doc.array();
    } else if (doc.isObject() && doc.object().contains("data")) {
        cartas = doc.object().value("data").toArray();
    } else {
        qDebug() << "Formato de JSON nao reconhecido (esperado array ou objeto com chave 'data').";
        return -1;
    }

    int importadas = 0;
    for (const QJsonValue &cartaValue : cartas) {
        QJsonObject obj = cartaValue.toObject();

        QSqlQuery insere;
        insere.prepare(
            "INSERT OR REPLACE INTO cartas "
            "(id, name, supertype, type, attacks, text, evolvesTo, weakness, resistance, rarity, artist) "
            "VALUES (:id, :name, :supertype, :type, :attacks, :text, :evolvesTo, :weakness, :resistance, :rarity, :artist)"
        );

        insere.bindValue(":id", obj.value("id").toString());
        insere.bindValue(":name", obj.value("name").toString());
        insere.bindValue(":supertype", obj.value("supertype").toString());
        insere.bindValue(":type", obj.contains("types")
                                       ? jsonValueToString(obj.value("types"))
                                       : obj.value("type").toString());
        insere.bindValue(":attacks", jsonValueToString(obj.value("attacks")));
        insere.bindValue(":text", jsonValueToString(obj.value("text")));
        insere.bindValue(":evolvesTo", jsonValueToString(obj.value("evolvesTo")));
        insere.bindValue(":weakness", obj.contains("weaknesses")
                                            ? jsonValueToString(obj.value("weaknesses"))
                                            : obj.value("weakness").toString());
        insere.bindValue(":resistance", obj.contains("resistances")
                                              ? jsonValueToString(obj.value("resistances"))
                                              : obj.value("resistance").toString());
        insere.bindValue(":rarity", obj.value("rarity").toString());
        insere.bindValue(":artist", obj.value("artist").toString());

        //Anoto o numero de cartas importadas
        if (insere.exec()) {
            importadas++;
        } else {
            qDebug() << "Falha ao inserir carta:" << insere.lastError().text();
        }
    }
    return importadas;
}

//Procuro os arquivos no computador
int NexusDbManager::ingestCardsFromJsonFile(const QString &filePath) {
    QFile arquivo(filePath);
    if (!arquivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Nao foi possivel abrir o arquivo:" << filePath;
        return -1;
    }
    QByteArray dados = arquivo.readAll();
    arquivo.close();
    return ingestCardsFromJson(dados);
}

//Filtro que pesquisa a carta
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

//Adiciona carta no estoque com o preço
bool NexusDbManager::addCardToStock(const QString &cardId, int quantidade, double preco) {
    QSqlQuery insere;
    insere.prepare("INSERT OR REPLACE INTO estoque (carta_id, quantidade, preco) VALUES (:id, :qtd, :preco)");
    insere.bindValue(":id", cardId);
    insere.bindValue(":qtd", quantidade);
    insere.bindValue(":preco", preco);
    return insere.exec();
}

//Conto todos os itens do estoque
int NexusDbManager::stockItemCount() {
    QSqlQuery query("SELECT COUNT(*) FROM estoque");
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}
