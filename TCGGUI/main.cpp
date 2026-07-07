/**
 * @file main.cpp
 * @brief Ponto de entrada do executável TCGGUI.
 *
 * Inicializa o banco de dados (via Singleton NexusDbManager, da DLL TCGCore),
 * exibe o LoginDialog e, de acordo com o perfil autenticado, abre a
 * DevPortalWindow (ADMIN) ou a StoreDashWindow (LOJISTA).
 */

#include <QApplication>
#include "logindialog.h"
#include "devportalwindow.h"
#include "storedashwindow.h"
#include "nexusdbmanager.h" // Integrando com a DLL do Core (Singleton)

/**
 * @brief Função principal da aplicação TCGGUI.
 * @param argc Quantidade de argumentos de linha de comando.
 * @param argv Vetor de argumentos de linha de comando.
 * @return int Código de saída da aplicação Qt.
 */
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Inicializa o banco de dados
    NexusDbManager::getInstance().initDatabase();

    int codigoRetorno = 1000;

    while (codigoRetorno == 1000) {
        LoginDialog login;

        // Se o login for aceito:
        if (login.exec() == QDialog::Accepted) {

            // Faz o roteamento
            if (login.getPerfil() == "ADMIN") {
                DevPortalWindow *devPortal = new DevPortalWindow();
                devPortal->setAttribute(Qt::WA_DeleteOnClose);
                devPortal->show();
            } else {
                StoreDashWindow *storeDash = new StoreDashWindow();
                storeDash->setAttribute(Qt::WA_DeleteOnClose);
                storeDash->show();
            }

            // O programa fica travado aqui, rodando a janela escolhida.
            // Quando alguém clicar em "Sair", mandamos o código 1000,
            // ele sai dessa linha e repete o while lá em cima!
            codigoRetorno = a.exec();

        } else {
            // O usuário clicou no "X" para fechar o login, encerra tudo.
            break;
        }
    }

    return codigoRetorno;
}
