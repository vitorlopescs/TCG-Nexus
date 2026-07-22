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
 */
class LoginDialog : public QDialog {
    Q_OBJECT
public:
    /**
     * @brief Constrói o dialog de login e monta o layout de widgets.
     * @param parent Widget pai (opcional, padrão nullptr).
     */
    explicit LoginDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("TCG Nexus - Autenticação");
        resize(320, 220); // Altura reajustada para suportar os novos botões

        auto *layout = new QVBoxLayout(this);
        txtLogin = new QLineEdit(this);
        txtLogin->setObjectName("txtLogin");
        txtLogin->setPlaceholderText("E-mail (padrão inicial: admin@tcgnexus.com)");

        txtSenha = new QLineEdit(this);
        txtSenha->setObjectName("txtSenha");
        txtSenha->setEchoMode(QLineEdit::Password);
        txtSenha->setPlaceholderText("Senha (padrão inicial: 123)");

        btnEntrar = new QPushButton("Entrar no Nexus", this);
        btnEntrar->setObjectName("btnEntrar");

        // Botões requeridos pelos Storyboards
        btnEsqueciSenha = new QPushButton("Esqueci minha senha", this);
        btnCriarConta = new QPushButton("Criar conta", this);

        layout->addWidget(new QLabel("<b>TCG Nexus</b> — Hub de Integração TCG", this));
        layout->addWidget(txtLogin);
        layout->addWidget(txtSenha);
        layout->addWidget(btnEntrar);
        layout->addWidget(btnEsqueciSenha);
        layout->addWidget(btnCriarConta);

        connect(btnEntrar, &QPushButton::clicked, this, &LoginDialog::tentarLogin);
    }

    /**
     * @brief Retorna o perfil autenticado na última tentativa de login bem-sucedida.
     * @return QString "ADMIN" ou "LOJISTA". Vazio se ainda não houve login com sucesso.
     */
    QString getPerfil() const { return perfil; }

private slots:
    /**
     * @brief Slot disparado ao clicar em "Entrar no Nexus".
     */
    void tentarLogin() {
        if(txtLogin->text().trimmed().isEmpty() || txtSenha->text().trimmed().isEmpty()){
            QMessageBox::warning(this, "Erro", "Preencha todos os campos");
            return;
        }

        QString perfilEncontrado = NexusDbManager::getInstance().authenticateUser(txtLogin->text(), txtSenha->text());
        
        if (!perfilEncontrado.isEmpty()) {
            perfil = perfilEncontrado;
            accept();
        } else {
            // Mensagem de erro validada no cenário BDD
            QMessageBox::warning(this, "Erro", "Credenciais inválidas");
            txtSenha->clear();
            txtSenha->setFocus();
        }
    }

private:
    QLineEdit *txtLogin;           ///< Campo de entrada do e-mail.
    QLineEdit *txtSenha;           ///< Campo de entrada da senha (modo password).
    QPushButton *btnEntrar;        ///< Botão que dispara a tentativa de login.
    QPushButton *btnEsqueciSenha;  ///< Botão para recuperação de senha (Storyboard).
    QPushButton *btnCriarConta;    ///< Botão para criação de nova conta (Storyboard).
    QString perfil;                ///< Perfil autenticado: "ADMIN", "LOJISTA" ou vazio.
};
#endif // LOGINDIALOG_H