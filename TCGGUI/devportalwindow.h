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
#include <QTableWidget>
#include <QHeaderView>
#include "nexusdbmanager.h"
#include "logindialog.h"

/**
 * @file devportalwindow.h
 * @brief Janela do módulo DevPortal: ingestão de dados oficiais, cadastro e gestão de usuários.
 */

class DevPortalWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit DevPortalWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Nexus DevPortal - Administração");
        resize(550, 650); // Altura aumentada para comportar a tabela perfeitamente

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

        // --- GRUPO 3: Gestão de Usuários (Tabela) ---
        auto *groupLista = new QGroupBox("Gestão de Usuários (Inativar Acesso)", this);
        auto *layoutLista = new QVBoxLayout(groupLista);
        
        tabelaUsuarios = new QTableWidget(groupLista);
        tabelaUsuarios->setObjectName("tabelaUsuarios");
        tabelaUsuarios->setColumnCount(4);
        tabelaUsuarios->setHorizontalHeaderLabels({"ID", "Nome", "E-mail", "Status"});

        btnInativar = new QPushButton("Inativar Usuário Selecionado", groupLista);
        btnInativar->setObjectName("btnInativar");
        
        btnEditarPermissao = new QPushButton("Alterar Nível de Acesso (Admin <-> Lojista)", groupLista);
        btnEditarPermissao->setObjectName("btnEditarPermissao");
        
        layoutLista->addWidget(tabelaUsuarios);
        layoutLista->addWidget(btnEditarPermissao); // <--- Botão adicionado
        layoutLista->addWidget(btnInativar);
        
        // Ajustes visuais da tabela
        tabelaUsuarios->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); 
        tabelaUsuarios->setSelectionBehavior(QAbstractItemView::SelectRows); 
        tabelaUsuarios->setSelectionMode(QAbstractItemView::SingleSelection);
        
        btnInativar = new QPushButton("Inativar Usuário Selecionado", groupLista);
        btnInativar->setObjectName("btnInativar");
        
        layoutLista->addWidget(tabelaUsuarios);
        layoutLista->addWidget(btnInativar);

        // --- MONTANDO O LAYOUT FINAL ---
        layout->addWidget(groupImport);
        layout->addWidget(groupUsuarios);
        layout->addWidget(groupLista);

        auto *btnVoltar = new QPushButton("Sair / Voltar", central);
        layout->addWidget(btnVoltar);

        setCentralWidget(central);

        // --- INICIALIZAÇÃO DE DADOS ---
        carregarUsuarios();

        // --- CONEXÕES (SIGNAIS E SLOTS) ---
        
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
                carregarUsuarios(); // Recarrega a tabela imediatamente após o cadastro
            } else {
                QMessageBox::warning(this, "Erro", "E-mail já está em uso");
            }
        });

        connect(btnInativar, &QPushButton::clicked, [this]() {
            int row = tabelaUsuarios->currentRow();
            if(row < 0) {
                QMessageBox::warning(this, "Aviso", "Selecione um usuário na tabela para inativar.");
                return;
            }
            
            // Extrai o ID da primeira coluna da linha selecionada
            int idTarget = tabelaUsuarios->item(row, 0)->text().toInt();
            
            // Simula o ID do Administrador logado atual (neste contexto MVP local)
            int idLogado = NexusDbManager::getInstance().getLoggedUserId("admin@tcgnexus.com"); 
            
            if(NexusDbManager::getInstance().deactivateUser(idTarget, idLogado)) {
                QMessageBox::information(this, "Sucesso", "Acesso de usuário inativado!");
                carregarUsuarios(); // Recarrega a tabela imediatamente após inativar
            } else {
                QMessageBox::warning(this, "Erro", "Não é possível inativar a própria conta");
            }
        });

        connect(btnEditarPermissao, &QPushButton::clicked, [this]() {
            int row = tabelaUsuarios->currentRow();
            if(row < 0) {
                QMessageBox::warning(this, "Aviso", "Selecione um usuário na tabela.");
                return;
            }
            
            int idTarget = tabelaUsuarios->item(row, 0)->text().toInt();
            QString perfilAtual = tabelaUsuarios->item(row, 3)->text(); // A coluna 3 exibe o Perfil
            
            int idLogado = NexusDbManager::getInstance().getLoggedUserId("admin@tcgnexus.com"); 
            if(idTarget == idLogado) {
                QMessageBox::warning(this, "Erro", "Não é possível alterar as próprias permissões de acesso.");
                return;
            }

            // Alterna a permissão
            QString novoPerfil = (perfilAtual == "ADMIN") ? "LOJISTA" : "ADMIN";
            
            if(NexusDbManager::getInstance().updateUserProfile(idTarget, novoPerfil)) {
                QMessageBox::information(this, "Sucesso", QString("Permissão atualizada para %1!").arg(novoPerfil));
                carregarUsuarios();
            }
        });

        connect(btnVoltar, &QPushButton::clicked, this, [this]() {
            this->close();
            qApp->exit(1000);
        });
    }

private:
    QPushButton *btnImportarJson;      
    QLabel *lblStatusImportacao;       
    QLineEdit *txtNovoNome;            
    QLineEdit *txtNovoEmail;           
    QLineEdit *txtNovaSenha;           
    QComboBox *comboPerfil;            
    QPushButton *btnCadastrarUsuario;  
    
    QTableWidget *tabelaUsuarios;      
    QPushButton *btnInativar;     
    QPushButton *btnEditarPermissao;     

    /**
     * @brief Limpa e popula a tabela de usuários com os dados atualizados do banco.
     */
    void carregarUsuarios() {
        tabelaUsuarios->setColumnCount(5);
        tabelaUsuarios->setHorizontalHeaderLabels({"ID", "Nome", "E-mail", "Perfil", "Status"});
        tabelaUsuarios->setRowCount(0); 
        auto users = NexusDbManager::getInstance().getAllUsers();
        
        for(int i = 0; i < users.size(); ++i) {
            tabelaUsuarios->insertRow(i);
            
            auto *itemId = new QTableWidgetItem(users[i]["id"].toString());
            auto *itemNome = new QTableWidgetItem(users[i]["nome"].toString());
            auto *itemEmail = new QTableWidgetItem(users[i]["email"].toString());
            auto *itemPerfil = new QTableWidgetItem(users[i]["perfil"].toString());
            auto *itemStatus = new QTableWidgetItem(users[i]["ativo"].toInt() == 1 ? "Ativo" : "Inativo");
            
            itemId->setFlags(itemId->flags() & ~Qt::ItemIsEditable);
            itemNome->setFlags(itemNome->flags() & ~Qt::ItemIsEditable);
            itemEmail->setFlags(itemEmail->flags() & ~Qt::ItemIsEditable);
            itemPerfil->setFlags(itemPerfil->flags() & ~Qt::ItemIsEditable);
            itemStatus->setFlags(itemStatus->flags() & ~Qt::ItemIsEditable);

            tabelaUsuarios->setItem(i, 0, itemId);
            tabelaUsuarios->setItem(i, 1, itemNome);
            tabelaUsuarios->setItem(i, 2, itemEmail);
            tabelaUsuarios->setItem(i, 3, itemPerfil);
            tabelaUsuarios->setItem(i, 4, itemStatus);
        }
    }
};
#endif // DEVPORTALWINDOW_H