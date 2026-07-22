#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include "nexusdbmanager.h"

/**
 * @file registerdialog.h
 * @brief Tela de auto-cadastro para novos usuários (Storyboard 1).
 */
class RegisterDialog : public QDialog {
    Q_OBJECT
public:
    explicit RegisterDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Nexus - Criar Conta");
        resize(300, 200);
        auto *layout = new QVBoxLayout(this);
        
        txtNome = new QLineEdit(this);
        txtNome->setObjectName("txtNome");
        txtNome->setPlaceholderText("Nome completo");
        
        txtEmail = new QLineEdit(this);
        txtEmail->setObjectName("txtEmail");
        txtEmail->setPlaceholderText("E-mail");

        txtSenha = new QLineEdit(this);
        txtSenha->setObjectName("txtSenha");
        txtSenha->setEchoMode(QLineEdit::Password);
        txtSenha->setPlaceholderText("Senha");
        
        btnCriar = new QPushButton("Criar conta", this);
        btnCriar->setObjectName("btnCriar");
        
        layout->addWidget(new QLabel("Preencha seus dados para solicitar acesso", this));
        layout->addWidget(txtNome);
        layout->addWidget(txtEmail);
        layout->addWidget(txtSenha);
        layout->addWidget(btnCriar);

        connect(btnCriar, &QPushButton::clicked, [this]() {
            if (txtNome->text().isEmpty() || txtEmail->text().isEmpty() || txtSenha->text().isEmpty()) {
                QMessageBox::warning(this, "Erro", "Preencha todos os campos.");
                return;
            }
            
            // O auto-cadastro cria a conta com privilégios de LOJISTA por padrão
            bool ok = NexusDbManager::getInstance().registerUser(
                txtNome->text(), 
                txtEmail->text(), 
                txtSenha->text(), 
                "LOJISTA"
            );

            if (ok) {
                QMessageBox::information(this, "Sucesso", "Conta criada com sucesso. Faça login!");
                accept();
            } else {
                QMessageBox::warning(this, "Erro", "E-mail já está em uso");
            }
        });
    }
private:
    QLineEdit *txtNome;
    QLineEdit *txtEmail;
    QLineEdit *txtSenha;
    QPushButton *btnCriar;
};
#endif // REGISTERDIALOG_H