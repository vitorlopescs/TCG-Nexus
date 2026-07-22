#ifndef DEVPORTALWINDOW_H
#define DEVPORTALWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QWidget>
#include "nexusdbmanager.h"
#include "logindialog.h"

/**
 * @file devportalwindow.h
 * @brief Janela do módulo DevPortal: ingestão de dados oficiais e cadastro de usuários.
 */

/**
 * @class DevPortalWindow
 * @brief Janela principal do perfil ADMIN.
 */
class DevPortalWindow : public QMainWindow {
    Q_OBJECT
public:
    /**
     * @brief Constrói a janela do DevPortal e monta o layout de widgets.
     * @param parent Widget pai (opcional, padrão nullptr).
     */
    explicit DevPortalWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Nexus DevPortal - Administração");
        resize(440, 380);

        auto *central = new QWidget(this);
        auto *layout = new QVBoxLayout(central);

        auto *groupImport = new QGroupBox("Sincronizar Base de Dados (Requisito 1)", this);
        auto *layoutImport = new QVBoxLayout(groupImport);
        btnImportarJson = new QPushButton("Importar Arquivo JSON de Cartas", groupImport);
        btnImportarJson->setObjectName("btnImportarJson");
        lblStatusImportacao = new QLabel("Nenhum arquivo importado ainda.", groupImport);
        lblStatusImportacao->setObjectName("lblStatusImportacao");
        layoutImport->addWidget(btnImportarJson);
        layoutImport->addWidget(lblStatusImportacao);

        auto *groupUsuarios = new QGroupBox("Cadastro de Usuários (Requisitos 4 e 5)", this);
        auto *layoutUsuarios = new QVBoxLayout(groupUsuarios);
        
        txtNovoNome = new QLineEdit(groupUsuarios);
        txtNovoNome->setObjectName("txtNovoNome");
        txtNovoNome->setPlaceholderText("Nome completo");

        txtNovoEmail = new QLineEdit(groupUsuarios);
        txtNovoEmail->setObjectName("txtNovoEmail");
        txtNovoEmail->setPlaceholderText("E-mail");
        
        txtNovaSenha = new QLineEdit(groupUsuarios);
        txtNovaSenha->setObjectName("txtNovaSenha");
        txtNovaSenha->setEchoMode(QLineEdit::Password);
        txtNovaSenha->setPlaceholderText("Senha");
        
        comboPerfil = new QComboBox(groupUsuarios);
        comboPerfil->setObjectName("comboPerfil");
        comboPerfil->addItem("LOJISTA");
        comboPerfil->addItem("ADMIN");
        
        btnCadastrarUsuario = new QPushButton("Cadastrar Usuário", groupUsuarios);
        btnCadastrarUsuario->setObjectName("btnCadastrarUsuario");

        layoutUsuarios->addWidget(txtNovoNome);
        layoutUsuarios->addWidget(txtNovoEmail);
        layoutUsuarios->addWidget(txtNovaSenha);
        layoutUsuarios->addWidget(comboPerfil);
        layoutUsuarios->addWidget(btnCadastrarUsuario);

        layout->addWidget(groupImport);
        layout->addWidget(groupUsuarios);
        setCentralWidget(central);

        connect(btnImportarJson, &QPushButton::clicked, [this]() {
            QString caminho = QFileDialog::getOpenFileName(this, "Selecionar dataset de cartas", QString(), "JSON (*.json)");
            if (caminho.isEmpty()) return;

            int total = NexusDbManager::getInstance().ingestCardsFromJsonFile(caminho);
            if (total >= 0) {
                lblStatusImportacao->setText(QString("%1 cartas importadas com sucesso.").arg(total));
            } else {
                lblStatusImportacao->setText("Falha ao importar o arquivo. Verifique o formato do JSON.");
            }
        });

        connect(btnCadastrarUsuario, &QPushButton::clicked, [this]() {
            if (txtNovoNome->text().isEmpty() || txtNovoEmail->text().isEmpty() || txtNovaSenha->text().isEmpty()) {
                QMessageBox::warning(this, "Erro", "Informe nome completo, e-mail e senha.");
                return;
            }
            bool ok = NexusDbManager::getInstance().registerUser(
                txtNovoNome->text(), 
                txtNovoEmail->text(), 
                txtNovaSenha->text(), 
                comboPerfil->currentText()
            );

            if (ok) {
                QMessageBox::information(this, "Sucesso", "Usuário cadastrado com sucesso!");
                txtNovoNome->clear();
                txtNovoEmail->clear();
                txtNovaSenha->clear();
            } else {
                // Mensagem de erro corrigida segundo o BDD
                QMessageBox::warning(this, "Erro", "E-mail já está em uso");
            }
        });

        auto *btnVoltar = new QPushButton("Sair / Voltar", central);
        layout->addWidget(btnVoltar);

        connect(btnVoltar, &QPushButton::clicked, this, [this]() {
            this->close();
            qApp->exit(1000);
        });
    }

private:
    QPushButton *btnImportarJson;      ///< Botão que abre o seletor de arquivo JSON.
    QLabel *lblStatusImportacao;       ///< Rótulo com o resultado da última importação.
    QLineEdit *txtNovoNome;            ///< Campo de Nome completo do novo usuário.
    QLineEdit *txtNovoEmail;           ///< Campo de e-mail do novo usuário a cadastrar.
    QLineEdit *txtNovaSenha;           ///< Campo de senha do novo usuário a cadastrar.
    QComboBox *comboPerfil;            ///< Seletor do perfil de acesso (LOJISTA/ADMIN).
    QPushButton *btnCadastrarUsuario;  ///< Botão que efetiva o cadastro do usuário.
};
#endif // DEVPORTALWINDOW_H