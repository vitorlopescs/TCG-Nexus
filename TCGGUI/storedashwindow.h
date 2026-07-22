#ifndef STOREDASHWINDOW_H
#define STOREDASHWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QWidget>
#include "nexusdbmanager.h"
#include "logindialog.h"

/**
 * @file storedashwindow.h
 * @brief Janela do módulo StoreDash: busca de cartas por atributo e gestão de estoque local.
 */

class StoreDashWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit StoreDashWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Nexus StoreDash - Gestão de Estoque");
        resize(500, 560);

        auto *central = new QWidget(this);
        auto *layout = new QVBoxLayout(central);

        auto *groupBusca = new QGroupBox("Buscar Carta por Atributo Técnico", this);
        auto *layoutBusca = new QVBoxLayout(groupBusca);

        comboAtributo = new QComboBox(groupBusca);
        comboAtributo->setObjectName("comboAtributo");
        comboAtributo->addItem("name");
        comboAtributo->addItem("type");
        comboAtributo->addItem("supertype");
        comboAtributo->addItem("rarity");
        comboAtributo->addItem("artist");

        txtValorBusca = new QLineEdit(groupBusca);
        txtValorBusca->setObjectName("txtValorBusca");
        txtValorBusca->setPlaceholderText("Valor a buscar (ex: Pikachu)");

        btnBuscar = new QPushButton("Buscar no Catálogo Oficial", groupBusca);
        btnBuscar->setObjectName("btnBuscar");

        listResultados = new QListWidget(groupBusca);
        listResultados->setObjectName("listResultados");

        layoutBusca->addWidget(comboAtributo);
        layoutBusca->addWidget(txtValorBusca);
        layoutBusca->addWidget(btnBuscar);
        layoutBusca->addWidget(listResultados);

        auto *groupEstoque = new QGroupBox("Adicionar ao Estoque Local", this);
        auto *layoutEstoque = new QHBoxLayout(groupEstoque);

        spinQuantidade = new QSpinBox(groupEstoque);
        spinQuantidade->setObjectName("spinQuantidade");
        spinQuantidade->setMinimum(0);
        spinQuantidade->setMaximum(9999);

        spinPreco = new QDoubleSpinBox(groupEstoque);
        spinPreco->setObjectName("spinPreco");
        spinPreco->setPrefix("R$ ");
        spinPreco->setMaximum(999999.99);

        btnAdicionarEstoque = new QPushButton("Adicionar ao Estoque", groupEstoque);
        btnAdicionarEstoque->setObjectName("btnAdicionarEstoque");

        layoutEstoque->addWidget(spinQuantidade);
        layoutEstoque->addWidget(spinPreco);
        layoutEstoque->addWidget(btnAdicionarEstoque);

        auto *groupEditarPreco = new QGroupBox("Atualizar Preço Unitário (Item Selecionado)", this);
        auto *layoutEditar = new QHBoxLayout(groupEditarPreco);
        
        spinNovoPreco = new QDoubleSpinBox(groupEditarPreco);
        spinNovoPreco->setObjectName("spinNovoPreco");
        spinNovoPreco->setPrefix("R$ ");
        spinNovoPreco->setMinimum(-9999.00); 
        spinNovoPreco->setMaximum(999999.99);
        
        btnAtualizarPreco = new QPushButton("Atualizar Preço", groupEditarPreco);
        btnAtualizarPreco->setObjectName("btnAtualizarPreco");
        
        layoutEditar->addWidget(spinNovoPreco);
        layoutEditar->addWidget(btnAtualizarPreco);

        lblStatusEstoque = new QLabel(this);
        lblStatusEstoque->setObjectName("lblStatusEstoque");

        int itensAtuais = NexusDbManager::getInstance().stockItemCount();
        if (itensAtuais > 0) {
            lblStatusEstoque->setText(QString("Seu estoque possui %1 tipos de cartas diferentes.").arg(itensAtuais));
        } else {
            lblStatusEstoque->setText("Nenhum item adicionado ao estoque ainda.");
        }

        layout->addWidget(groupBusca);
        layout->addWidget(groupEstoque);
        layout->addWidget(groupEditarPreco);
        layout->addWidget(lblStatusEstoque);
        setCentralWidget(central);

        connect(btnBuscar, &QPushButton::clicked, [this]() {
            listResultados->clear();
            auto resultados = NexusDbManager::getInstance().searchCardsByAttribute(comboAtributo->currentText(), txtValorBusca->text());
            for (const auto &item : resultados) {
                QString linha = QString("%1 | %2 (%3)").arg(item.value("id").toString(), item.value("name").toString(), item.value("type").toString());
                auto *entrada = new QListWidgetItem(linha, listResultados);
                entrada->setData(Qt::UserRole, item.value("id").toString());
            }
            if (resultados.isEmpty()) {
                lblStatusEstoque->setText("Nenhuma carta encontrada para esse atributo/valor.");
            }
        });

        connect(btnAdicionarEstoque, &QPushButton::clicked, [this]() {
            auto *itemSelecionado = listResultados->currentItem();
            if (!itemSelecionado) {
                QMessageBox::warning(this, "Erro", "Selecione uma carta na lista de resultados.");
                return;
            }
            
            if (spinQuantidade->value() <= 0) {
                QMessageBox::warning(this, "Erro", "A quantidade deve ser maior que zero");
                return;
            }

            QString cardId = itemSelecionado->data(Qt::UserRole).toString();
            bool ok = NexusDbManager::getInstance().addCardToStock(cardId, spinQuantidade->value(), spinPreco->value());
            if (ok) {
                lblStatusEstoque->setText(QString("Carta adicionada ao estoque com sucesso! %1 unidades adicionadas")
                                              .arg(spinQuantidade->value()));
            } else {
                lblStatusEstoque->setText("Falha ao adicionar item ao estoque.");
            }
        });

        connect(btnAtualizarPreco, &QPushButton::clicked, [this](){
            auto *itemSelecionado = listResultados->currentItem();
            if (!itemSelecionado) {
                QMessageBox::warning(this, "Erro", "Selecione uma carta para alterar o preço.");
                return;
            }

            if (spinNovoPreco->value() < 0) {
                QMessageBox::warning(this, "Erro", "Valor inválido. Insira um preço positivo");
                return;
            }

            QString cardId = itemSelecionado->data(Qt::UserRole).toString();
            bool ok = NexusDbManager::getInstance().updateCardPrice(cardId, spinNovoPreco->value());
            if(ok){
                QMessageBox::information(this, "Sucesso", "Preço atualizado com sucesso!");
                lblStatusEstoque->setText("Preço unitário alterado.");
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
    QComboBox *comboAtributo;           
    QLineEdit *txtValorBusca;           
    QPushButton *btnBuscar;             
    QListWidget *listResultados;        
    QSpinBox *spinQuantidade;           
    QDoubleSpinBox *spinPreco;          
    QPushButton *btnAdicionarEstoque;   
    QDoubleSpinBox *spinNovoPreco;      
    QPushButton *btnAtualizarPreco;     
    QLabel *lblStatusEstoque;           
};
#endif // STOREDASHWINDOW_H