/**
 * @file test_storedash.cpp
 * @brief Teste automatizado de GUI (QTest) do módulo StoreDash.
 */

#include <QtTest/QtTest>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QMessageBox>
#include "storedashwindow.h"
#include "nexusdbmanager.h"

class TesteStoreDash : public QObject {
    Q_OBJECT

private slots:
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

    void testarValidacaoDeQuantidadeEAdicao() {
        StoreDashWindow tela;
        tela.show();
        QVERIFY(QTest::qWaitForWindowExposed(&tela));

        auto *comboAtributo = tela.findChild<QComboBox*>("comboAtributo");
        auto *txtValorBusca = tela.findChild<QLineEdit*>("txtValorBusca");
        auto *btnBuscar = tela.findChild<QPushButton*>("btnBuscar");
        auto *listResultados = tela.findChild<QListWidget*>("listResultados");
        auto *spinQuantidade = tela.findChild<QSpinBox*>("spinQuantidade");
        auto *spinPreco = tela.findChild<QDoubleSpinBox*>("spinPreco");
        auto *btnAdicionarEstoque = tela.findChild<QPushButton*>("btnAdicionarEstoque");
        auto *lblStatusEstoque = tela.findChild<QLabel*>("lblStatusEstoque");

        comboAtributo->setCurrentText("name");
        txtValorBusca->setText("Pikachu");
        QTest::mouseClick(btnBuscar, Qt::LeftButton);
        QTest::qWait(100);

        QCOMPARE(listResultados->count(), 1);
        listResultados->setCurrentRow(0);

        spinQuantidade->setValue(0);
        spinPreco->setValue(10.00);
        
        QTest::mouseClick(btnAdicionarEstoque, Qt::LeftButton); 
        QCOMPARE(NexusDbManager::getInstance().stockItemCount(), 0);

        spinQuantidade->setValue(10);
        QTest::mouseClick(btnAdicionarEstoque, Qt::LeftButton);
        QTest::qWait(100);
        QVERIFY(lblStatusEstoque->text().contains("unidades adicionadas"));
        QCOMPARE(NexusDbManager::getInstance().stockItemCount(), 1);
    }

    void testarAtualizacaoDePreco() {
        StoreDashWindow tela;
        tela.show();
        QVERIFY(QTest::qWaitForWindowExposed(&tela));

        auto *comboAtributo = tela.findChild<QComboBox*>("comboAtributo");
        auto *txtValorBusca = tela.findChild<QLineEdit*>("txtValorBusca");
        auto *btnBuscar = tela.findChild<QPushButton*>("btnBuscar");
        auto *listResultados = tela.findChild<QListWidget*>("listResultados");
        auto *spinNovoPreco = tela.findChild<QDoubleSpinBox*>("spinNovoPreco");
        auto *btnAtualizarPreco = tela.findChild<QPushButton*>("btnAtualizarPreco");

        comboAtributo->setCurrentText("name");
        txtValorBusca->setText("Pikachu");
        QTest::mouseClick(btnBuscar, Qt::LeftButton);
        QTest::qWait(100);
        listResultados->setCurrentRow(0);

        spinNovoPreco->setValue(-10.00);
        QTest::mouseClick(btnAtualizarPreco, Qt::LeftButton);

        spinNovoPreco->setValue(150.00);
        QTest::mouseClick(btnAtualizarPreco, Qt::LeftButton);
        QTest::qWait(100);
    }
};

int runTesteStoreDash(int argc, char *argv[]) {
    TesteStoreDash tc;
    return QTest::qExec(&tc, argc, argv);
}
#include "test_storedash.moc"
