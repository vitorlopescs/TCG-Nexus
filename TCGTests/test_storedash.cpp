/**
 * @file test_storedash.cpp
 * @brief Teste automatizado de GUI (QTest) do módulo StoreDash.
 *
 * Valida o Requisito 2 da Sprint 1: buscar uma carta por atributo técnico e
 * adicioná-la ao estoque local com quantidade e preço definidos. Para isolar
 * o teste de um arquivo externo em disco, o catálogo é semeado diretamente via
 * NexusDbManager::ingestCardsFromJson(), simulando uma ingestão já concluída.
 */

#include <QtTest/QtTest>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include "storedashwindow.h"
#include "nexusdbmanager.h"

/**
 * @class TesteStoreDash
 * @brief Suíte de testes de GUI (QTest) para a tela StoreDashWindow.
 */
class TesteStoreDash : public QObject {
    Q_OBJECT

private slots:
    /**
     * @brief Prepara o banco em memória e semeia uma carta de teste antes da suíte,
     * simulando a conclusão do Requisito 1 (ingestão de metadados JSON).
     */
    void initTestCase() {
        NexusDbManager::getInstance().initDatabase();

        QByteArray jsonCartaTeste = R"(
        [
            {
                "id": "base1-58",
                "name": "Pikachu",
                "supertype": "Pokémon",
                "types": ["Lightning"],
                "attacks": [{"name": "Gnaw", "text": "Ataque básico de mordida."}],
                "rarity": "Common",
                "artist": "Mitsuhiro Arita"
            }
        ]
        )";

        int importadas = NexusDbManager::getInstance().ingestCardsFromJson(jsonCartaTeste);
        QVERIFY(importadas == 1);
    }

    /**
     * @brief Verifica se a busca por atributo técnico encontra a carta esperada
     * e se a adição ao estoque local é concluída com sucesso.
     *
     * Simula: selecionar o atributo "name", digitar "Pikachu", clicar em
     * "Buscar no Catálogo Oficial", selecionar o resultado, definir
     * quantidade/preço e clicar em "Adicionar ao Estoque".
     */
    void testarBuscaPorAtributoEAdicaoAoEstoque() {
        StoreDashWindow tela;
        tela.show();
        QTest::qWaitForWindowExposed(&tela);

        auto *comboAtributo = tela.findChild<QComboBox*>("comboAtributo");
        auto *txtValorBusca = tela.findChild<QLineEdit*>("txtValorBusca");
        auto *btnBuscar = tela.findChild<QPushButton*>("btnBuscar");
        auto *listResultados = tela.findChild<QListWidget*>("listResultados");
        auto *spinQuantidade = tela.findChild<QSpinBox*>("spinQuantidade");
        auto *spinPreco = tela.findChild<QDoubleSpinBox*>("spinPreco");
        auto *btnAdicionarEstoque = tela.findChild<QPushButton*>("btnAdicionarEstoque");
        auto *lblStatusEstoque = tela.findChild<QLabel*>("lblStatusEstoque");

        QVERIFY(comboAtributo != nullptr);
        QVERIFY(btnBuscar != nullptr);
        QVERIFY(listResultados != nullptr);
        QVERIFY(btnAdicionarEstoque != nullptr);

        // Busca por atributo "name" = "Pikachu"
        comboAtributo->setCurrentText("name");
        txtValorBusca->setText("Pikachu");
        QTest::mouseClick(btnBuscar, Qt::LeftButton);
        QTest::qWait(100);

        QCOMPARE(listResultados->count(), 1);
        listResultados->setCurrentRow(0);

        spinQuantidade->setValue(10);
        spinPreco->setValue(25.90);
        QTest::mouseClick(btnAdicionarEstoque, Qt::LeftButton);
        QTest::qWait(100);

        QVERIFY(lblStatusEstoque->text().contains("sucesso"));
        QCOMPARE(NexusDbManager::getInstance().stockItemCount(), 1);
    }
};

QTEST_MAIN(TesteStoreDash)
#include "test_storedash.moc"
