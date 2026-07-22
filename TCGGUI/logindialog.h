#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include "nexusdbmanager.h"
#include "recoverdialog.h"
#include "registerdialog.h"

/**
 * @file logindialog.h
 * @brief Tela de autenticação do TCG Nexus (define o perfil ADMIN ou LOJISTA).
 */

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("TCG Nexus - Autenticação");
        resize(320, 220);

        auto *layout = new QVBoxLayout(this);
        txtLogin = new QLineEdit(this);
        txtLogin->setObjectName("txtLogin");
        txtLogin->setPlaceholderText("E-mail (padrão inicial: admin@tcgnexus.com)");

        txtSenha = new QLineEdit(this);
        txtSenha->setObjectName("txtSenha");
        txtSenha->setEchoMode(QLineEdit::Password);
        txtSenha->setPlaceholderText("Senha (padrão inicial: senha123)");

        btnEntrar = new QPushButton("Entrar no Nexus", this);
        btnEntrar->setObjectName("btnEntrar");

        btnEsqueciSenha = new QPushButton("Esqueci minha senha", this);
        btnCriarConta = new QPushButton("Criar conta", this);

        layout->addWidget(new QLabel("<b>TCG Nexus</b> — Hub de Integração TCG", this));
        layout->addWidget(txtLogin);
        layout->addWidget(txtSenha);
        layout->addWidget(btnEntrar);
        layout->addWidget(btnEsqueciSenha);
        layout->addWidget(btnCriarConta);

        connect(btnEntrar, &QPushButton::clicked, this, &LoginDialog::tentarLogin);
        
        connect(btnEsqueciSenha, &QPushButton::clicked, [this](){ 
            RecoverDialog r; 
            r.exec(); 
        }); 

        connect(btnCriarConta, &QPushButton::clicked, [this](){ 
            RegisterDialog reg; 
            reg.exec(); 
        }); 
    }

    QString getPerfil() const { return perfil; }

private slots:
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
            QMessageBox::warning(this, "Erro", "Credenciais inválidas");
            txtSenha->clear();
            txtSenha->setFocus();
        }
    }

private:
    QLineEdit *txtLogin;           
    QLineEdit *txtSenha;           
    QPushButton *btnEntrar;        
    QPushButton *btnEsqueciSenha;  
    QPushButton *btnCriarConta;    
    QString perfil;                
};
#endif // LOGINDIALOG_H