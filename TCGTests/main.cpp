#include <QApplication>
#include <QtTest>

// Declaração das funções que rodam cada suíte de testes
int runTesteStoreDash(int argc, char *argv[]);
int runTesteLogin(int argc, char *argv[]);
int runTesteDevPortal(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    // Instancia a aplicação gráfica necessária para os testes de UI
    QApplication app(argc, argv);
    int status = 0;

    // Executa os testes sequencialmente
    status |= runTesteStoreDash(argc, argv);
    status |= runTesteLogin(argc, argv);
    status |= runTesteDevPortal(argc, argv);

    return status;
}