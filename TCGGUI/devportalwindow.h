#ifndef DEVPORTALWINDOW_H
#define DEVPORTALWINDOW_H

#include <QMainWindow>
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

/**
 * @file devportalwindow.h
 * @brief Janela do módulo DevPortal: ingestão de dados oficiais e cadastro de usuários.
 */

/**
 * @class DevPortalWindow
 * @brief Janela principal do perfil ADMIN.
 *
 * Concentra dois casos de uso da Sprint 1: sincronizar a base de dados de
 * cartas a partir de um arquivo JSON (Requisito 1) e cadastrar novos usuários
 * com seu respectivo perfil de acesso (Requisitos 4 e 5).
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
        resize(440, 340);

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
        txtNovoLogin = new QLineEdit(groupUsuarios);
        txtNovoLogin->setObjectName("txtNovoLogin");
        txtNovoLogin->setPlaceholderText("Novo usuário");
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

        layoutUsuarios->addWidget(txtNovoLogin);
        layoutUsuarios->addWidget(txtNovaSenha);
        layoutUsuarios->addWidget(comboPerfil);
        layoutUsuarios->addWidget(btnCadastrarUsuario);

        layout->addWidget(groupImport);
        layout->addWidget(groupUsuarios);
        setCentralWidget(central);

        // Abre um seletor de arquivo, delega o parse/ingestão ao NexusDbManager
        // (Requisito 1) e reporta quantas cartas foram importadas.
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

        // Cadastra um novo usuário (Requisito 4) já com o perfil de acesso
        // definido (Requisito 5).
        connect(btnCadastrarUsuario, &QPushButton::clicked, [this]() {
            if (txtNovoLogin->text().isEmpty() || txtNovaSenha->text().isEmpty()) {
                QMessageBox::warning(this, "Erro", "Informe usuário e senha.");
                return;
            }
            bool ok = NexusDbManager::getInstance().registerUser(txtNovoLogin->text(), txtNovaSenha->text(), comboPerfil->currentText());
            if (ok) {
                QMessageBox::information(this, "Sucesso", "Usuário cadastrado com sucesso!");
                txtNovoLogin->clear();
                txtNovaSenha->clear();
            } else {
                QMessageBox::warning(this, "Erro", "Usuário já existe.");
            }
        });
    }

private:
    QPushButton *btnImportarJson;      ///< Botão que abre o seletor de arquivo JSON.
    QLabel *lblStatusImportacao;       ///< Rótulo com o resultado da última importação.
    QLineEdit *txtNovoLogin;           ///< Campo de login do novo usuário a cadastrar.
    QLineEdit *txtNovaSenha;           ///< Campo de senha do novo usuário a cadastrar.
    QComboBox *comboPerfil;            ///< Seletor do perfil de acesso (LOJISTA/ADMIN).
    QPushButton *btnCadastrarUsuario;  ///< Botão que efetiva o cadastro do usuário.
};
#endif // DEVPORTALWINDOW_H
