#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TAPE_SIZE 256
#define MAX_CANDIDATES 500 // Primeras 35 candidatas shortlex

typedef struct {
    char tape1[TAPE_SIZE]; // Cinta 1: Generador
    char tape2[TAPE_SIZE]; // Cinta 2: Decisor
    char tape3[TAPE_SIZE]; // Cinta 3: Impresora
    int head1;
    int head2;
    int head3;
} TuringMachine;

// Inicializa las cintas y cabezales
void init_tm(TuringMachine *tm) {
    memset(tm->tape1, 0, TAPE_SIZE);
    memset(tm->tape2, 0, TAPE_SIZE);
    memset(tm->tape3, 0, TAPE_SIZE);
    
    // La Cinta 3 inicia con el separador '#'
    tm->tape3[0] = '#';
    tm->head1 = 0;
    tm->head2 = 0;
    tm->head3 = 1;
    tm->tape3[tm->head3] = '\0'; // CORRECCIÓN 1: Asegura fin de cadena inicial
}

// Generador en orden shortlex
void generate_candidate(TuringMachine *tm, int index) {
    memset(tm->tape1, 0, TAPE_SIZE);
    tm->head1 = 0;

    if (index == 1) {
        tm->tape1[0] = '\0'; // Cadena vacía (epsilon)
        return;
    }

    int temp = index;
    char buffer[TAPE_SIZE];
    int pos = 0;

    while (temp > 1) {
        buffer[pos++] = (temp % 2 == 0) ? '0' : '1';
        temp /= 2;
    }

    for (int i = 0; i < pos; i++) {
        tm->tape1[i] = buffer[pos - 1 - i];
    }
    tm->tape1[pos] = '\0';
}

// Copiar candidata a la Cinta 2 y reposicionar cabezal
void copy_tape1_to_tape2(TuringMachine *tm) {
    memset(tm->tape2, 0, TAPE_SIZE);
    int len = strlen(tm->tape1);
    for (int i = 0; i < len; i++) {
        tm->tape2[i] = tm->tape1[i];
    }
    tm->tape2[len] = '\0';
    tm->head2 = 0; // Reposicionamiento del cabezal al inicio
}

// Decisor D para L2 = {0^n 1^n}
bool run_decider(TuringMachine *tm) {
    // Cadena vacía (n = 0) pertenece a L2
    if (tm->tape2[0] == '\0') {
        return true;
    }

    // Fase 1: Verificación de Formato (0*1*)
    int state = 0;
    int i = 0;
    while (tm->tape2[i] != '\0') {
        char symbol = tm->tape2[i];
        if (state == 0) {
            if (symbol == '1') state = 1;
            else if (symbol != '0') return false;
        } else if (state == 1) {
            if (symbol == '0') return false; // Rechaza '0' después de '1'
        }
        i++;
    }

    // Fase 2: Emparejamiento mediante tachado ('x')
    while (1) {
        int zero_idx = -1;
        for (int j = 0; tm->tape2[j] != '\0'; j++) {
            if (tm->tape2[j] == '0') {
                zero_idx = j;
                break;
            }
        }

        int one_idx = -1;
        for (int j = 0; tm->tape2[j] != '\0'; j++) {
            if (tm->tape2[j] == '1') {
                one_idx = j;
                break;
            }
        }

        if (zero_idx == -1 && one_idx == -1) return true;  // q_accept
        if (zero_idx != -1 && one_idx == -1) return false; // q_reject (sobran 0s)
        if (zero_idx == -1 && one_idx != -1) return false; // q_reject (sobran 1s)

        tm->tape2[zero_idx] = 'x';
        tm->tape2[one_idx] = 'x';
    }
}

// Impresora: Escribe la cadena aceptada en Cinta 3
void print_to_tape3(TuringMachine *tm) {
    int len = strlen(tm->tape1);
    for (int i = 0; i < len; i++) {
        tm->tape3[tm->head3++] = tm->tape1[i];
    }
    tm->tape3[tm->head3++] = '#';
    tm->tape3[tm->head3] = '\0'; // CORRECCIÓN 2: Mantiene la cadena terminada en nulo
}

void display_trace(TuringMachine *tm, int iter, const char *cadena, bool aceptada) {
    printf("Iteracion %2d | Candidata: \"%-6s\" | Decisor: %-8s | Cinta 3: %s\n",
           iter, cadena, aceptada ? "ACEPTA" : "RECHAZA", tm->tape3);
}

int main() {
    TuringMachine tm;
    init_tm(&tm);

    printf("===================================================================\n");
    printf(" SIMULACION DE MT ENUMERADORA PARA L2 = {0^n 1^n : n >= 0}\n");
    printf("===================================================================\n\n");

    for (int i = 1; i <= MAX_CANDIDATES; i++) {
        generate_candidate(&tm, i);
        copy_tape1_to_tape2(&tm);
        bool accepted = run_decider(&tm);

        if (accepted) {
            print_to_tape3(&tm);
        }

        display_trace(&tm, i, tm.tape1, accepted);
    }

    printf("\n===================================================================\n");
    printf("RESULTADO FINAL EN CINTA DE SALIDA (CINTA 3):\n");
    printf("%s\n", tm.tape3);
    printf("===================================================================\n");

    return 0;
}