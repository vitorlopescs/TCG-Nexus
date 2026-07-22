#include <QtTest/QtTest>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include "devportalwindow.h"
#include "nexusdbmanager.h"

class TesteDevPortal : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() { NexusDbManager::getInstance().initDatabase(); }

    void testarCadastroEInativacaoUsuarios() {
        DevPortalWindow tela;
        auto *txtNovoNome = tela.findChild<QLineEdit*>("txtNovoNome");
        auto *txtNovoEmail = tela.findChild<QLineEdit*>("txtNovoEmail");
        auto *txtNovaSenha = tela.findChild<QLineEdit*>("txtNovaSenha");
        auto *btnCadastrarUsuario = tela.findChild<QPushButton*>("btnCadastrarUsuario");
        auto *tabelaUsuarios = tela.findChild<QTableWidget*>("tabelaUsuarios");

        // Cenário 1: Cadastro
        txtNovoNome->setText("João Lojista");
        txtNovoEmail->setText("joao@loja.com");
        txtNovaSenha->setText("senha123");
        QTest::mouseClick(btnCadastrarUsuario, Qt::LeftButton);
        
        // Falha (Duplicado)
        txtNovoNome->setText("Maria Admin");
        QTest::mouseClick(btnCadastrarUsuario, Qt::LeftButton);
        
        // Cenário 2: Inativar usuário existente (O admin é id 1, Joao é id 2)
        // Seleciona a linha do joão na tabela e clica no botão inativar (lógica testada diretamente no banco):
        int idJoao = NexusDbManager::getInstance().getLoggedUserId("joao@loja.com");
        int idAdmin = NexusDbManager::getInstance().getLoggedUserId("admin@tcgnexus.com");
        
        QVERIFY(NexusDbManager::getInstance().deactivateUser(idJoao, idAdmin) == true);
        QVERIFY(NexusDbManager::getInstance().deactivateUser(idAdmin, idAdmin) == false); // Falha ao tentar desativar a si mesmo
    }
};
QTEST_MAIN(TesteDevPortal)
#include "test_devportal.moc"