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

/**
 * @class StoreDashWindow
 * @brief Janela principal do perfil LOJISTA.
 *
 * Implementa o Requisito 2 da Sprint 1: permitir que o lojista pesquise uma
 * carta no catálogo oficial por um atributo técnico (nome, tipo, raridade,
 * etc.) e a adicione ao estoque local informando quantidade e preço de venda.
 */
class StoreDashWindow : public QMainWindow {
    Q_OBJECT
public:
    /**
     * @brief Constrói a janela do StoreDash e monta o layout de widgets.
     * @param parent Widget pai (opcional, padrão nullptr).
     */
    //Inicia a tela do lojista
    explicit StoreDashWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Nexus StoreDash - Gestão de Estoque");
        resize(480, 440);

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
        spinQuantidade->setMinimum(1);
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
        layout->addWidget(lblStatusEstoque);
        setCentralWidget(central);

        // Consulta o catálogo oficial (via NexusDbManager) pelo atributo e valor
        // selecionados, listando os resultados encontrados.
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

        // Adiciona a carta selecionada na lista de resultados ao estoque local,
        // com a quantidade e o preço informados.
        connect(btnAdicionarEstoque, &QPushButton::clicked, [this]() {
            auto *itemSelecionado = listResultados->currentItem();
            if (!itemSelecionado) {
                QMessageBox::warning(this, "Erro", "Selecione uma carta na lista de resultados.");
                return;
            }
            QString cardId = itemSelecionado->data(Qt::UserRole).toString();
            bool ok = NexusDbManager::getInstance().addCardToStock(cardId, spinQuantidade->value(), spinPreco->value());
            if (ok) {
                lblStatusEstoque->setText(QString("%1 unidades adicionadas! Total de tipos de cartas no estoque: %2")
                                              .arg(spinQuantidade->value())
                                              .arg(NexusDbManager::getInstance().stockItemCount()));
            } else {
                lblStatusEstoque->setText("Falha ao adicionar item ao estoque.");
            }
        });

        auto *btnVoltar = new QPushButton("Sair / Voltar", central);

        layout->addWidget(btnVoltar);

        connect(btnVoltar, &QPushButton::clicked, this, [this]() {
            this->close();

            qApp->exit(1000);
        });

    }

// atributos
private:
    QComboBox *comboAtributo;           ///< Seletor do atributo técnico usado na busca.
    QLineEdit *txtValorBusca;           ///< Campo com o valor (ou trecho) a buscar.
    QPushButton *btnBuscar;             ///< Botão que dispara a busca no catálogo.
    QListWidget *listResultados;        ///< Lista com as cartas encontradas na busca.
    QSpinBox *spinQuantidade;           ///< Quantidade de unidades a adicionar ao estoque.
    QDoubleSpinBox *spinPreco;          ///< Preço de venda definido pelo lojista.
    QPushButton *btnAdicionarEstoque;   ///< Botão que efetiva a adição ao estoque.
    QLabel *lblStatusEstoque;           ///< Rótulo com o resultado da última operação de estoque.
};
#endif // STOREDASHWINDOW_H
