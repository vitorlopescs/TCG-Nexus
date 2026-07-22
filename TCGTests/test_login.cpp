#include <QtTest/QtTest>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include "logindialog.h"
#include "recoverdialog.h"
#include "nexusdbmanager.h"

class TesteLogin : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() { NexusDbManager::getInstance().initDatabase(); }

    void testarValidacaoDeCredenciais() {
        LoginDialog tela;
        auto *txtLogin = tela.findChild<QLineEdit*>("txtLogin");
        auto *txtSenha = tela.findChild<QLineEdit*>("txtSenha");
        auto *btnEntrar = tela.findChild<QPushButton*>("btnEntrar");

        txtLogin->setText("");
        QTest::mouseClick(btnEntrar, Qt::LeftButton);
        QVERIFY(tela.getPerfil().isEmpty());

        txtLogin->setText("lojista@tcgnexus.com");
        txtSenha->setText("errada");
        QTest::mouseClick(btnEntrar, Qt::LeftButton);
        QVERIFY(tela.getPerfil().isEmpty());
    }

    void testarRecuperacaoDeSenha() {
        RecoverDialog tela;
        auto *txtEmail = tela.findChild<QLineEdit*>("txtEmail");
        auto *btnEnviar = tela.findChild<QPushButton*>("btnEnviar");

        txtEmail->setText("admin@tcgnexus.com");
        QTest::mouseClick(btnEnviar, Qt::LeftButton);
        
        txtEmail->setText("naoexiste@tcgnexus.com");
        QTest::mouseClick(btnEnviar, Qt::LeftButton);
    }
};
int runTesteLogin(int argc, char *argv[]) {
    TesteLogin tc;
    return QTest::qExec(&tc, argc, argv);
}
#include "test_login.moc"