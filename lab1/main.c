#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

int main() {
    Node* root = NULL;
    int choice, value;

    while (1) {
        printf("\nМЕНЮ\n");
        printf("1. Добавить узел\n");
        printf("2. Визуализировать дерево\n");
        printf("3. Удалить узел\n");
        printf("4. Найти лист с мин. глубиной\n");
        printf("5. Выход\n");
        printf("Выберите действие: ");

        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                printf("Введите число для добавления: ");
                scanf("%d", &value);
                root = insert(root, value);
                printf("Узел добавлен.\n");
                break;
            case 2:
                if (root == NULL) printf("Дерево пусто.\n");
                else {
                    printf("Дерево (повёрнуто на 90 град):\n");
                    printTree(root, 0);
                }
                break;
            case 3:
                printf("Введите число для удаления: ");
                scanf("%d", &value);
                root = deleteNode(root, value);
                printf("Удаление завершено.\n");
                break;
            case 4:
                taskVariant4(root);
                break;
            case 5:
                freeTree(root);
                printf("Выход из программы.\n");
                return 0;
            default:
                printf("Неверный выбор.\n");
        }
    }
    return 0;
}