#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include "nexusdbmanager.h"

/**
 * @file logindialog.h
 * @brief Tela de autenticação do TCG Nexus (define o perfil ADMIN ou LOJISTA).
 */

/**
 * @class LoginDialog
 * @brief Dialog modal de autenticação exibido ao iniciar a aplicação.
 *
 * Valida as credenciais contra o hash armazenado no SQLite via
 * NexusDbManager::authenticateUser() (Requisito 3). Em caso de sucesso, o
 * perfil retornado ("ADMIN" ou "LOJISTA") pode ser consultado via getPerfil()
 * para decidir qual janela principal abrir.
 */
class LoginDialog : public QDialog {
    Q_OBJECT
public:
    /**
     * @brief Constrói o dialog de login e monta o layout de widgets.
     * @param parent Widget pai (opcional, padrão nullptr).
     */
    //Criacao da tela
    explicit LoginDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("TCG Nexus - Autenticação");
        resize(320, 170);

        auto *layout = new QVBoxLayout(this);
        txtLogin = new QLineEdit(this);
        txtLogin->setObjectName("txtLogin");
        txtLogin->setPlaceholderText("Usuário (padrão inicial: admin)");

        txtSenha = new QLineEdit(this);
        txtSenha->setObjectName("txtSenha");
        txtSenha->setEchoMode(QLineEdit::Password);
        txtSenha->setPlaceholderText("Senha (padrão inicial: 123)");

        btnEntrar = new QPushButton("Entrar no Nexus", this);
        btnEntrar->setObjectName("btnEntrar");

        layout->addWidget(new QLabel("<b>TCG Nexus</b> — Hub de Integração TCG", this));
        layout->addWidget(txtLogin);
        layout->addWidget(txtSenha);
        layout->addWidget(btnEntrar);

        //Tento fazer login
        connect(btnEntrar, &QPushButton::clicked, this, &LoginDialog::tentarLogin);
    }

    /**
     * @brief Retorna o perfil autenticado na última tentativa de login bem-sucedida.
     * @return QString "ADMIN" ou "LOJISTA". Vazio se ainda não houve login com sucesso.
     */
    //retorna o perfil
    QString getPerfil() const { return perfil; }

private slots:
    /**
     * @brief Slot disparado ao clicar em "Entrar no Nexus".
     *
     * Delega a validação para NexusDbManager::authenticateUser(). Em caso de
     * sucesso, guarda o perfil e chama accept(); caso contrário, exibe a
     * mensagem "Login Invalido" (conforme critério de aceitação do Requisito 3).
     */
    //Tento fazer o login
    void tentarLogin() {
        //Pego o perfil
        QString perfilEncontrado = NexusDbManager::getInstance().authenticateUser(txtLogin->text(), txtSenha->text());
        //Se tiver, aceita
        if (!perfilEncontrado.isEmpty()) {
            perfil = perfilEncontrado;
            accept();
        //Senao existir, da erro
        } else {
            QMessageBox::warning(this, "Erro", "Login Invalido");
            txtSenha->clear();
            txtSenha->setFocus();
        }
    }

    //atrivutos do login
private:
    QLineEdit *txtLogin;      ///< Campo de entrada do usuário.
    QLineEdit *txtSenha;      ///< Campo de entrada da senha (modo password).
    QPushButton *btnEntrar;   ///< Botão que dispara a tentativa de login.
    QString perfil;           ///< Perfil autenticado: "ADMIN", "LOJISTA" ou vazio.
};
#endif // LOGINDIALOG_H


//teste de commit 
