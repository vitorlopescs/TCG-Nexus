#ifndef RECOVERDIALOG_H
#define RECOVERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include "nexusdbmanager.h"

/**
 * @file recoverdialog.h
 * @brief Tela para redefinição de senha (Requisito BDD).
 */

/**
 * @class RecoverDialog
 * @brief Dialog modal para solicitação de redefinição de senha.
 */
class RecoverDialog : public QDialog {
    Q_OBJECT
public:
    /**
     * @brief Constrói a interface de recuperação de acesso.
     * @param parent Widget pai (opcional, padrão nullptr).
     */
    explicit RecoverDialog(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Nexus - Recuperar Senha");
        resize(300, 150);
        auto *layout = new QVBoxLayout(this);
        
        txtEmail = new QLineEdit(this);
        txtEmail->setObjectName("txtEmail");
        txtEmail->setPlaceholderText("E-mail cadastrado");
        
        btnEnviar = new QPushButton("Enviar link", this);
        btnEnviar->setObjectName("btnEnviar");
        
        layout->addWidget(new QLabel("Recuperação de Acesso", this));
        layout->addWidget(txtEmail);
        layout->addWidget(btnEnviar);

        connect(btnEnviar, &QPushButton::clicked, [this]() {
            if(NexusDbManager::getInstance().checkEmailExists(txtEmail->text())) {
                QMessageBox::information(this, "Sucesso", "Link de redefinição enviado ao e-mail!");
                accept();
            } else {
                QMessageBox::warning(this, "Erro", "E-mail não encontrado no sistema");
            }
        });
    }
private:
    QLineEdit *txtEmail;
    QPushButton *btnEnviar;
};
#endif // RECOVERDIALOG_H