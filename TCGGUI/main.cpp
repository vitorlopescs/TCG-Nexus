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

    // Inicializa o banco de dados usando o Singleton exportado pela DLL TCGCore
    NexusDbManager::getInstance().initDatabase();

    LoginDialog login;
    if (login.exec() == QDialog::Accepted) {
        if (login.getPerfil() == "ADMIN") {
            DevPortalWindow devPortal;
            devPortal.show();
            return a.exec();
        } else {
            StoreDashWindow storeDash;
            storeDash.show();
            return a.exec();
        }
    }
    return 0;
}
